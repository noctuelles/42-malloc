/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 14:02:59 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/28 16:31:20 by plouvel          ###   ########.fr       */
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

#include "block.h"
#include "malloc_utils.h"
#include "pool.h"
#include "pthread.h"
#include "tunable.h"

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
    pthread_mutex_lock(&g_lock);

    static bool is_init = false;
    size_t      i       = 0;

    if (is_init) {
        pthread_mutex_unlock(&g_lock);
        return (0);
    }
    configure_pools_tunable();
    while (i < N_POOLS) {
        if (init_pool(&g_pools[i]) == -1) {
            pthread_mutex_unlock(&g_lock);
            return (-1);
        }
        i++;
    }
    is_init = true;
    pthread_mutex_unlock(&g_lock);
    return (0);
}

void *
malloc_normal(void *blk, t_pool *blk_pool, size_t adj_size) {
    size_t ext_size = 0;

    if (blk == NULL) {
        ext_size = MAX(adj_size, get_tunable(FT_POOL_CHUNK_EXTENSION_STR, POOL_CHUNK_EXTENSION));
        blk      = extend_pool(blk_pool, ext_size / WORD_SIZE);
        if (blk == (void *)-1) {
            return (NULL);
        }
    }
    place_blk(&blk_pool->head, blk, adj_size);
    return (blk);
}

void *
malloc(size_t size) {
    size_t  adj_size = 0;
    t_pool *blk_pool = NULL;
    void   *blk      = NULL;

    if (init_malloc() == -1) {
        return (NULL);
    }
    if (size == 0) {
        size = 1;
    }
    adj_size = ADJ_ALLOC_SIZE(size);
    pthread_mutex_lock(&g_lock);
    blk = find_fit_in_pools(g_pools, N_POOLS, adj_size, &blk_pool);
    if (blk_pool == NULL) {
        pthread_mutex_unlock(&g_lock);
        return (new_orphean_blk(adj_size));
    } else {
        blk = malloc_normal(blk, blk_pool, adj_size);
        if (blk == NULL) {
            pthread_mutex_unlock(&g_lock);
            return (new_orphean_blk(adj_size));
        }
    }
    pthread_mutex_unlock(&g_lock);
    return (blk);
}

/* ## REALLOC ## */

static void *
new_malloc_cpy_old_data(void *old_ptr, size_t old_size, size_t new_size) {
    void *new_ptr = NULL;

    new_ptr = malloc(new_size);
    if (new_ptr == NULL) {
        return (NULL);
    }
    memcpy(new_ptr, old_ptr, old_size);
    free(old_ptr);
    return (new_ptr);
}

static void *
realloc_orphean(void *blk, size_t adj_size) {
    size_t blk_size = GET_ORPHEAN_SIZE(GET_HDR(blk));

    if (adj_size < blk_size) {
        return (blk);
    }
    if (adj_size > blk_size) {
        return (new_malloc_cpy_old_data(blk, blk_size, adj_size));
    }
    return (blk);
}

static void *
realloc_normal(void *blk, size_t adj_size) {
    size_t blk_size = GET_SIZE(GET_HDR(blk));

    if (adj_size < blk_size) {
        return (blk);
    }
    if (adj_size > blk_size) {
        return (new_malloc_cpy_old_data(blk, blk_size, adj_size));
    }
    return (blk);
}

void *
realloc(void *ptr, size_t size) {
    if (ptr == NULL) {
        return (malloc(size));
    }
    if (size == 0) {
        free(ptr);
        return (NULL);
    }
    if (GET_ORPHEAN(GET_HDR(ptr))) {
        ptr = realloc_orphean(ptr, ADJ_ALLOC_SIZE(size));
    } else {
        ptr = realloc_normal(ptr, ADJ_ALLOC_SIZE(size));
    }
    return (ptr);
}

/* ## FREE ## */

static void
free_orphean(void *blk) {
    void *blk_header = GET_HDR(blk);
    munmap(GET_ORPHEAN_BASE(blk_header), GET_ORPHEAN_SIZE(blk_header));
}

static void
free_normal(void *blk) {
    t_pool *blk_pool = NULL;

    pthread_mutex_lock(&g_lock);
    blk_pool = find_blk_in_pools(g_pools, N_POOLS, blk);
    PUT_WORD(GET_HDR(blk), PACK(GET_SIZE(GET_HDR(blk)), FREE));
    PUT_WORD(GET_FTR(blk), PACK(GET_SIZE(GET_HDR(blk)), FREE));
    coalesce_blk(&blk_pool->head, blk);
    pthread_mutex_unlock(&g_lock);
}

void
free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    if (GET_ORPHEAN(GET_HDR(ptr))) {
        return (free_orphean(ptr));
    } else {
        return (free_normal(ptr));
    }
}

/* ## CALLOC ## */

void *
calloc(size_t nmemb, size_t size) {
    size_t total_size = 0;
    void  *ptr        = NULL;

    total_size = nmemb * size;
    ptr        = malloc(total_size);
    if (ptr == NULL) {
        return (NULL);
    }
    bzero(ptr, total_size);
    return (ptr);
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