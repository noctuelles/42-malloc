/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 14:02:59 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/29 16:08:07 by plouvel          ###   ########.fr       */
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

static void *
realloc_block(void *ptr, size_t size) {
    size_t  blk_size = 0;
    size_t  adj_size = 0;
    t_pool *blk_pool = NULL;

    if (ptr == NULL) {
        return (malloc_block(size));
    }
    if (size == 0) {
        free_block(ptr);
        return (NULL);
    }
    adj_size = ADJ_ALLOC_SIZE(size);
    blk_pool = find_blk_in_pools(g_pools, N_POOLS, ptr);
    if (blk_pool == NULL) {
        blk_size = GET_ORPHEAN_SIZE(GET_HDR(ptr));
    } else {
        blk_size = GET_SIZE(GET_HDR(ptr));
    }
    (void)blk_pool;
    if (adj_size < blk_size) {
        return (ptr);
    }
    void *new_ptr = malloc_block(adj_size);
    if (!new_ptr) {
        return (NULL);
    }
    memcpy(new_ptr, ptr, blk_size);
    return (new_ptr);
}

void *
realloc(void *ptr, size_t size) {
    void *new_ptr = NULL;

    pthread_mutex_lock(&g_lock);
    new_ptr = realloc_block(ptr, size);
    pthread_mutex_unlock(&g_lock);

    return (new_ptr);
}

void
free(void *ptr) {
    pthread_mutex_lock(&g_lock);
    free_block(ptr);
    pthread_mutex_unlock(&g_lock);
}

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