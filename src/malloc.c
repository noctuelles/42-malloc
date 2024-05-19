/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 14:02:59 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/19 11:13:36 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdint.h>
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
malloc(size_t size) {
    size_t  adj_size = ADJ_ALLOC_SIZE(size);
    t_pool *pool     = NULL;
    void   *blk      = NULL;
    size_t  i        = 0;

    if (init_malloc() == -1 || size == 0) {
        return (NULL);
    }
    while (i < N_POOLS) {
        pool = &g_pools[i];
        if (adj_size >= pool->min_alloc_size && adj_size <= pool->max_alloc_size) {
            blk = find_fit_pool(pool, adj_size);
            break;
        }
        i++;
    }
    (void)blk;
    return (NULL);
}

// void *
// realloc(void *ptr, size_t size) {
//     size            = ADJ_ALLOC_SIZE(size);
//     size_t blk_size = GET_SIZE(GET_HDR(ptr));

//     if (ptr == NULL) {
//         return (malloc(size));
//     }
//     if (size == 0) {
//         free(ptr);
//         return (NULL);
//     }
//     if (size == blk_size) {
//         return (ptr);
//     }
//     if (size < blk_size) {
//         // TODO: best to just cut the reminder.
//         free(ptr);
//         return (malloc(size));
//     }

//     if (GET_ALLOC(GET_HDR(NEXT_BLK(ptr))) == FREE) {
//     }
// }

void
free(void *ptr) {
    void *hdr_ptr = GET_HDR(ptr);

    if (ptr == NULL) {
        return;
    }
    if (GET_ANONYMOUS(hdr_ptr) == ANONYMOUS) {
        (void)munmap(hdr_ptr, GET_SIZE(hdr_ptr));
        return;
    }
}