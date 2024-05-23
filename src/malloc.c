/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 14:02:59 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/23 17:29:45 by plouvel          ###   ########.fr       */
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
    size_t      j       = 0;

    if (is_init) {
        return (0);
    }
    configure_pools_tunable();
    while (i < N_POOLS) {
        if (init_pool(&g_pools[i]) == -1) {
            while (j < i) {
                // TODO: release pool
                j++;
            }
            return (-1);
        }
        i++;
    }
    is_init = true;
    return (0);
}

void *
malloc(size_t size) {
    size_t  adj_size       = 0;
    size_t  extention_size = 0;
    t_pool *blk_pool       = NULL;
    void   *blk            = NULL;

    if (init_malloc() == -1) {
        return (NULL);
    }
    if (size == 0) {
        size = 1;
    }
    adj_size = ADJ_ALLOC_SIZE(size);
    if ((blk = find_fit_in_pools(g_pools, N_POOLS, adj_size, &blk_pool)) == NULL) {
        if (blk_pool != NULL) {
            extention_size = MAX(adj_size, get_tunable(FT_POOL_CHUNK_EXTENSION_STR, POOL_CHUNK_EXTENSION));
            if ((blk = extend_pool(blk_pool, extention_size / WORD_SIZE)) == (void *)-1) {
                return (NULL);
            }
        } else {
            blk = new_anonymous_blk(adj_size);
            return (blk);
        }
    }
    place_blk(&blk_pool->head, blk, adj_size);
    return (blk);
}

void *
realloc(void *ptr, size_t size) {
    size_t  blk_size = 0;
    size_t  adj_size = 0;
    t_pool *blk_pool = NULL;

    if (ptr == NULL) {
        return (malloc(size));
    }
    if (size == 0) {
        free(ptr);
        return (NULL);
    }
    adj_size = ADJ_ALLOC_SIZE(size);
    if ((blk_pool = find_blk_in_pools(g_pools, N_POOLS, ptr)) == NULL) {
        blk_size = GET_ANON_SIZE(GET_HDR(ptr));
    } else {
        blk_size = GET_SIZE(GET_HDR(ptr));
    }
    (void)blk_pool;
    if (adj_size < blk_size) {
        return (ptr);
    }
    void *new_ptr = malloc(adj_size);
    if (!new_ptr) {
        return (NULL);
    }
    memcpy(new_ptr, ptr, blk_size);
    return (new_ptr);
}

void
free(void *ptr) {
    t_pool *blk_pool = NULL;

    if (ptr == NULL) {
        return;
    }
    if ((blk_pool = find_blk_in_pools(g_pools, N_POOLS, ptr)) == NULL) {
        return (free_anonymous_blk(ptr));
    }
    PUT_WORD(GET_HDR(ptr), PACK(GET_SIZE(GET_HDR(ptr)), FREE));
    PUT_WORD(GET_FTR(ptr), PACK(GET_SIZE(GET_HDR(ptr)), FREE));
    coalesce_blk(&blk_pool->head, ptr);
}

void *
calloc(size_t nmemb, size_t size) {
    size_t total_size = nmemb * size;
    void  *ptr        = malloc(total_size);

    if (ptr == NULL) {
        return (NULL);
    }
    bzero(ptr, total_size);
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