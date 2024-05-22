/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 14:02:59 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/22 17:36:28 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
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

void *
my_memcpy(void *dest, const void *src, size_t n) {
    char       *d = dest;
    const char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

static void *
new_malloc_n_cpy(void *old_ptr, size_t old_size, size_t new_size) {
    void *new_ptr = NULL;

    new_ptr = malloc(new_size);
    if (new_ptr == NULL) {
        return (NULL);
    }
    my_memcpy(new_ptr, old_ptr, old_size);
    free(old_ptr);
    return (new_ptr);
}

void *
malloc(size_t size) {
    size_t  adj_size = ADJ_ALLOC_SIZE(size);
    size_t  ext_size = 0;
    t_pool *blk_pool = NULL;
    void   *blk      = NULL;

    if (init_malloc() == -1 || size == 0) {
        return (NULL);
    }
    if ((blk = find_fit_in_pools(g_pools, N_POOLS, adj_size, &blk_pool)) == NULL) {
        if (blk_pool != NULL) {
            ext_size = MAX(adj_size, POOL_CHUNK_EXTENSION);
            if ((blk = extend_pool(blk_pool, ext_size / WORD_SIZE)) == (void *)-1) {
                return (NULL);
            }
        } else {
            return (new_anonymous_blk(adj_size));
        }
    }
    place_blk(&blk_pool->head, blk, adj_size);
    return (blk);
}

void *
realloc(void *ptr, size_t size) {
    size_t  blk_size = 0;
    size_t  adj_size = ADJ_ALLOC_SIZE(size);
    t_pool *blk_pool = NULL;

    if (ptr == NULL) {
        return (malloc(size));
    }
    if (adj_size == 0) {
        free(ptr);
        return (NULL);
    }
    if ((blk_pool = find_blk_in_pools(g_pools, N_POOLS, ptr)) == NULL) {
        return (NULL);
    }
    blk_size = GET_SIZE(GET_HDR(ptr));
    if (adj_size < blk_size) {
        if (shrink_blk(&blk_pool->head, ptr, blk_size - adj_size) == NULL) {
            return (ptr);
        }
    } else if (adj_size > blk_size) {
        if (expand_blk(&blk_pool->head, ptr, adj_size - blk_size) == NULL) {
            return (new_malloc_n_cpy(ptr, blk_size, adj_size));
        }
    }
    return (ptr);
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
