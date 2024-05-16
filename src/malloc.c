/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 14:02:59 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/16 23:19:57 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdint.h>
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

/**
 * @brief find an appropriate free block for the requested size. If no free block is available, it returns NULL.
 *
 * @param size allocation size.
 * @return void* the free block or NULL.
 */
void *
find_appropriate_free_block(size_t size) {
    // TODO
    (void)size;
    return (NULL);
}

void *
malloc(size_t size) {
    if (init_malloc() == -1 || size == 0) {
        return (NULL);
    }
    return (NULL);
}

// void *
// realloc(void *ptr, size_t size) {}

void
free(void *ptr) {
    void   *hdr_ptr = GET_HDR(ptr);
    t_pool *pool    = NULL;

    if (ptr == NULL) {
        return;
    }
    if (GET_ANONYMOUS(hdr_ptr) == ANONYMOUS) {
        (void)munmap(hdr_ptr, GET_SIZE(hdr_ptr));
        return;
    }
    PUT_WORD(GET_HDR(ptr), PACK(GET_SIZE(GET_HDR(ptr)), FREE));
    PUT_WORD(GET_FTR(ptr), PACK(GET_SIZE(GET_HDR(ptr)), FREE));

    ptr = coalesce_block(ptr);

    PUT_DWORD(PREV_BLK(ptr), NULL);
    PUT_DWORD(NEXT_BLK(ptr), pool->free_list_head);
    PUT_DWORD(PREV_BLK(pool->free_list_head), ptr);

    pool->free_list_head = ptr;
}