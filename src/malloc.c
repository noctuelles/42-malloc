/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 14:02:59 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/30 16:04:19 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "block.h"
#include "malloc_utils.h"
#include "pool.h"
#include "pthread.h"
#include "tunable.h"
#include "utils.h"

static t_pool g_pools[N_POOLS] = {{
                                      .min_alloc_size = 0,
                                      .max_alloc_size = 0,
                                      .head           = NULL,
                                      .heap           = {0},
                                  },
                                  {
                                      .min_alloc_size = 0,
                                      .max_alloc_size = 0,
                                      .head           = NULL,
                                      .heap           = {0},
                                  }};

static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

/* ## MALLOC ## */

static void
configure_pools_tunable() {
    g_pools[0].min_alloc_size = ADJ_ALLOC_SIZE(POOL_ONE_MIN_ALLOC_SIZE);
    g_pools[0].max_alloc_size = ADJ_ALLOC_SIZE(get_tunable(FT_POOL_ONE_MAX_ALLOC_SIZE_STR, POOL_ONE_MAX_ALLOC_SIZE));

    g_pools[1].min_alloc_size = g_pools[0].max_alloc_size + 1;
    g_pools[1].max_alloc_size = ADJ_ALLOC_SIZE(get_tunable(FT_POOL_TWO_MAX_ALLOC_SIZE_STR, POOL_TWO_MAX_ALLOC_SIZE));

    assert(g_pools[1].min_alloc_size > g_pools[0].max_alloc_size);
    assert(g_pools[1].max_alloc_size > g_pools[0].max_alloc_size);
}

/**
 * @brief Initialize the allocator.
 *
 * @return int -1 in case of errors, 0 otherwise.
 */
static int
init_malloc() {
    static bool is_init = false;
    size_t      i       = 0;

    if (is_init) {
        return (0);
    }
    configure_pools_tunable();
    while (i < N_POOLS) {
        if (init_pool(&g_pools[i]) == -1) {
            return (-1);
        }
        i++;
    }
    is_init = true;
    return (0);
}

static void *
malloc_block(size_t size) {
    size_t  adj_size = 0;
    size_t  ext_size = 0;
    t_pool *blk_pool = NULL;
    void   *blk      = NULL;

    if (init_malloc() == -1) {
        return (NULL);
    }
    if (size == 0) {
        size = 1;
    }
    adj_size = ADJ_ALLOC_SIZE(size);
    blk      = find_fit_in_pools(g_pools, N_POOLS, adj_size, &blk_pool);
    if (blk_pool == NULL) {
        return (new_orphean_blk(adj_size));
    }
    if (blk == NULL) {
        ext_size = MAX(adj_size, get_tunable(FT_POOL_CHUNK_EXTENSION_STR, POOL_CHUNK_EXTENSION));
        blk      = extend_pool(blk_pool, ext_size / WORD_SIZE);
        if (blk == NULL) {
            return (new_orphean_blk(adj_size));
        }
    }
    place_blk(&blk_pool->head, blk, adj_size);
    return (blk);
}

void *
malloc(size_t size) {
    void *ptr = NULL;

    pthread_mutex_lock(&g_lock);
    ptr = malloc_block(size);
    pthread_mutex_unlock(&g_lock);

    return (ptr);
}

/* ## FREE ## */

static void
free_block(void *ptr) {
    t_pool *blk_pool = NULL;

    if (ptr == NULL) {
        return;
    }
    if (GET_ORPHEAN(GET_HDR(ptr))) {
        free_orphean_blk(ptr);
    } else {
        blk_pool = find_blk_in_pools(g_pools, N_POOLS, ptr);
        if (blk_pool != NULL) {
            PUT_WORD(GET_HDR(ptr), PACK(GET_SIZE(GET_HDR(ptr)), FREE));
            PUT_WORD(GET_FTR(ptr), PACK(GET_SIZE(GET_HDR(ptr)), FREE));
            coalesce_blk(&blk_pool->head, ptr);
        }
    }
}
void
free(void *ptr) {
    pthread_mutex_lock(&g_lock);
    free_block(ptr);
    pthread_mutex_unlock(&g_lock);
}

/* ## REALLOC ## */

static void *
new_malloc_cpy_old_data(void *old_ptr, size_t old_size, size_t new_size) {
    void  *new_ptr = NULL;
    size_t ncpy    = 0;

    new_ptr = malloc_block(new_size);
    if (new_ptr == NULL) {
        return (NULL);
    }
    ncpy = MIN(new_size, old_size);
    if (GET_ORPHEAN(GET_HDR(old_ptr))) {
        ncpy -= ORPHEAN_BLK_MISC_SIZE;
    } else {
        ncpy -= BLK_MISC_SIZE;
    }
    memcpy(new_ptr, old_ptr, ncpy);
    free_block(old_ptr);
    return (new_ptr);
}

static void *
realloc_orphean(void *ptr, size_t adj_size) {
    const size_t blk_size      = GET_ORPHEAN_SIZE(GET_HDR(ptr));
    const size_t sys_page_size = (size_t)sysconf(_SC_PAGE_SIZE);

    if (adj_size < blk_size) {
        if (blk_size - adj_size >= sys_page_size) {
            return (new_malloc_cpy_old_data(ptr, blk_size, adj_size));
        }
    }

    if (adj_size > blk_size) {
        return (new_malloc_cpy_old_data(ptr, blk_size, adj_size));
    }

    return (ptr);
}

static void *
realloc_non_orphean(void *ptr, size_t adj_size) {
    const size_t blk_size  = GET_SIZE(GET_HDR(ptr));
    t_pool      *blk_pool  = find_blk_in_pools(g_pools, N_POOLS, ptr);
    size_t       xpnd_size = 0;

    if (blk_pool == NULL) {
        return (NULL);
    }
    if (adj_size < blk_size) {
        if (can_shrink_blk(ptr, blk_size - adj_size, blk_pool->min_alloc_size)) {
            return (shrink_blk(&blk_pool->head, ptr, blk_size - adj_size));
        }
    } else if (adj_size > blk_size) {
        xpnd_size = adj_size - blk_size;
        if (can_expand_blk(ptr, xpnd_size, blk_pool->max_alloc_size)) {
            return (expand_blk(&blk_pool->head, ptr, xpnd_size));
        } else {
            return (new_malloc_cpy_old_data(ptr, blk_size, adj_size));
        }
    }

    return (ptr);
}

static void *
realloc_block(void *ptr, size_t size) {
    if (ptr == NULL) {
        return (malloc_block(size));
    }
    if (size == 0) {
        free_block(ptr);
        return (NULL);
    }
    if (GET_ORPHEAN(GET_HDR(ptr))) {
        ptr = realloc_orphean(ptr, ADJ_ALLOC_SIZE(size));
    } else {
        ptr = realloc_non_orphean(ptr, ADJ_ALLOC_SIZE(size));
    }
    return (ptr);
}

void *
realloc(void *ptr, size_t size) {
    void *new_ptr = NULL;

    pthread_mutex_lock(&g_lock);
    new_ptr = realloc_block(ptr, size);
    pthread_mutex_unlock(&g_lock);

    return (new_ptr);
}

/* ## CALLOC ## */

void *
calloc(size_t nmemb, size_t size) {
    size_t total_size = 0;
    void  *ptr        = NULL;

    total_size = nmemb * size;
    pthread_mutex_lock(&g_lock);
    ptr = malloc_block(total_size);
    if (ptr == NULL) {
        return (NULL);
    }
    bzero(ptr, total_size);
    pthread_mutex_unlock(&g_lock);
    return (ptr);
}

size_t
malloc_usable_size(void *ptr) {
    if (ptr == NULL) {
        return (0);
    }
    if (GET_ORPHEAN(GET_HDR(ptr))) {
        return (GET_ORPHEAN_SIZE(GET_HDR(ptr)));
    }
    return (GET_SIZE(GET_HDR(ptr)));
}

/* ## Utils ## */

void
show_alloc_mem() {
    size_t i = 0;

    while (i < N_POOLS) {
        print_pool(&g_pools[i], PRINT_ALLOC);
        i++;
    }
}

void
show_alloc_mem_hex() {
    size_t i = 0;

    while (i < N_POOLS) {
        print_pretty_pool(&g_pools[i], PRINT_ALLOC);
        i++;
    }
}

void
show_pools_mem() {
    size_t i = 0;

    while (i < N_POOLS) {
        print_pool(&g_pools[i], PRINT_ALLOC | PRINT_FREE);
        i++;
    }
}