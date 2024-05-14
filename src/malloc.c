/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 14:02:59 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/14 20:44:56 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>

#include "block.h"
#include "pool.h"

static t_pool g_pools[N_POOLS] = {{
                                      .min_alloc_size = POOL_ONE_MIN_ALLOC_SIZE,
                                      .max_alloc_size = POOL_ONE_MAX_ALLOC_SIZE,
                                      .beginning_ptr  = NULL,
                                      .size           = 0,
                                  },
                                  {
                                      .min_alloc_size = POOL_TWO_MIN_ALLOC_SIZE,
                                      .max_alloc_size = POOL_TWO_MAX_ALLOC_SIZE,
                                      .beginning_ptr  = NULL,
                                      .size           = 0,
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
                release_pool(&g_pools[i]);
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
    t_pool *targeted_pool = NULL;

    if (init_malloc() == -1 || size == 0) {
        return (NULL);
    }
}

void *
realloc(void *ptr, size_t size) {}

void
free(void *) {}

void
show_alloc_mem() {}