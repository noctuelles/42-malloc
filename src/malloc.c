/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 14:02:59 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/21 12:26:28 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

#include "block.h"
#include "pool.h"

static t_pool g_pools[N_POOLS] = {{
                                      .min_alloc_size = ADJ_ALLOC_SIZE(POOL_ONE_MIN_ALLOC_SIZE),
                                      .max_alloc_size = ADJ_ALLOC_SIZE(POOL_ONE_MAX_ALLOC_SIZE),
                                      .head           = NULL,
                                      .heap           = {0},
                                  },
                                  {
                                      .min_alloc_size = ADJ_ALLOC_SIZE(POOL_TWO_MIN_ALLOC_SIZE),
                                      .max_alloc_size = ADJ_ALLOC_SIZE(POOL_TWO_MAX_ALLOC_SIZE),
                                      .head           = NULL,
                                      .heap           = {0},
                                  }};

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

static void *
find_fit_in_pools(const size_t adj_size, t_pool **pool) {
    size_t i   = 0;
    void  *blk = NULL;

    while (i < N_POOLS) {
        *pool = &g_pools[i];
        if (adj_size >= (*pool)->min_alloc_size && adj_size <= (*pool)->max_alloc_size) {
            blk = find_fit_pool(*pool, adj_size);
            break;
        }
        i++;
    }
    return (blk);
}

static t_pool *
find_blk_pool(void *blk) {
    size_t blk_size = GET_SIZE(GET_HDR(blk));
    size_t i        = 0;

    while (i < N_POOLS) {
        if (blk_size >= g_pools[i].min_alloc_size && blk_size <= g_pools[i].max_alloc_size) {
            return (&g_pools[i]);
        }
        i++;
    }
    return (NULL);
}

void *
my_malloc(size_t size) {
    size_t  adj_size = ADJ_ALLOC_SIZE(size);
    t_pool *blk_pool = NULL;
    void   *blk      = NULL;

    if (init_malloc() == -1 || size == 0) {
        return (NULL);
    }
    if ((blk = find_fit_in_pools(adj_size, &blk_pool)) == NULL) {
        return (NULL);
    }
    place_blk(&blk_pool->head, blk, adj_size);
    print_pool(blk_pool, true);
    return (blk);
}

void *
my_realloc(void *ptr, size_t size) {
    size_t  blk_size     = GET_SIZE(GET_HDR(ptr));
    size_t  aligned_size = align_on_dword_boundary(size);
    t_pool *blk_pool     = NULL;
    void   *rslt         = NULL;

    if (ptr == NULL) {
        return (my_malloc(size));
    }
    if (aligned_size == 0) {
        my_free(ptr);
        return (NULL);
    }
    if ((blk_pool = find_blk_pool(ptr)) == NULL) {
        return (NULL);
    }
    if (aligned_size < blk_size) {
        if ((rslt = shrink_blk(&blk_pool->head, ptr, aligned_size)) == NULL) {
            return (ptr);
        }
    } else if (aligned_size > blk_size) {
        if ((rslt = expand_blk(&blk_pool->head, ptr, aligned_size)) != NULL) {
            return (rslt);
        }
        if ((rslt = my_malloc(aligned_size)) == NULL) {
            return (rslt);
        }
        memcpy(rslt, ptr, GET_PLD_SIZE(ptr));
        my_free(ptr);
    }
    return (rslt);
}

void
my_free(void *ptr) {
    t_pool *blk_pool = NULL;

    if (ptr == NULL) {
        return;
    }
    if ((blk_pool = find_blk_pool(ptr)) == NULL) {
        return;
    }
    PUT_WORD(GET_HDR(ptr), PACK(GET_SIZE(GET_HDR(ptr)), FREE));
    PUT_WORD(GET_FTR(ptr), PACK(GET_SIZE(GET_HDR(ptr)), FREE));
    coalesce_blk(&blk_pool->head, ptr);
    print_pool(blk_pool, true);
}