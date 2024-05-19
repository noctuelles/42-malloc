/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 21:53:47 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/19 11:14:46 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <assert.h>
#include <block.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "inc/malloc.h"
#include "malloc.h"

void *
find_fit_pool(const t_pool *pool, const size_t adj_size) {
    t_free_list *blk = pool->head;

    while (blk != NULL) {
        if (GET_SIZE(GET_HDR(blk)) >= adj_size) {
            return (blk);
        }
        blk = blk->next;
    }
    return (NULL);
}

/**
 * @brief Given a maximum allocation size, return the size of pool that is page size aligned.
 *
 * @param max_alloc_size
 * @return size_t
 */
size_t
get_adj_pool_size(size_t max_alloc_size) {
    const size_t page_size = (size_t)getpagesize();
    const size_t pool_size = MIN_ALLOC_PER_POOL * ADJ_ALLOC_SIZE(max_alloc_size) + (4 * WORD_SIZE);

    return (page_size * ((pool_size + page_size) / page_size));
}

void *
extend_pool(t_pool *pool, size_t words) {
    t_byte *heap_brk = NULL;
    void   *free_blk = NULL;

    words    = (words % 2) ? (words + 1) * WORD_SIZE : words * WORD_SIZE;
    heap_brk = sbrk_heap(&pool->heap, words);
    if (heap_brk == (void *)-1) {
        return (NULL);
    }
    PUT_WORD(GET_HDR(heap_brk), PACK(words, FREE));
    PUT_WORD(GET_FTR(heap_brk), PACK(words, FREE));
    PUT_WORD(GET_HDR(NEXT_BLK(heap_brk)), PACK(0, ALLOCATED));
    free_blk = coalesce_blk(&pool->head, heap_brk);
    return (free_blk);
}

int
init_pool(t_pool *pool) {
    t_byte *heap = NULL;

    heap = sbrk_heap(&pool->heap, 4);
    if (heap == (void *)-1) {
        return (-1);
    }
    PUT_WORD(heap, 0U);
    PUT_WORD(heap + (1 * WORD_SIZE), PACK(DWORD_SIZE, ALLOCATED));
    PUT_WORD(heap + (2 * WORD_SIZE), PACK(DWORD_SIZE, ALLOCATED));
    PUT_WORD(heap + (3 * WORD_SIZE), PACK(0, ALLOCATED));
    if (extend_pool(pool, 1 << 12) == NULL) {
        return (-1);
    }
    return (0);
}