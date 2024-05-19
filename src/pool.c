/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 21:53:47 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/19 21:05:05 by plouvel          ###   ########.fr       */
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
    size_t  bytes    = 0;

    bytes    = (words % 2) ? (words + 1) * WORD_SIZE : words * WORD_SIZE;
    heap_brk = sbrk_heap(&pool->heap, bytes);
    if (heap_brk == (void *)-1) {
        return (NULL);
    }
    PUT_WORD(GET_HDR(heap_brk), PACK(bytes, FREE));
    PUT_WORD(GET_FTR(heap_brk), PACK(bytes, FREE));
    PUT_WORD(GET_HDR(NEXT_BLK(heap_brk)), PACK(0, ALLOCATED));
    free_blk = coalesce_blk(&pool->head, heap_brk);
    return (free_blk);
}

int
init_pool(t_pool *pool) {
    t_byte *heap = NULL;

    if (init_heap(&pool->heap, (MIN_ALLOC_PER_POOL * pool->max_alloc_size) + 4 * WORD_SIZE) == (void *)-1) {
        return (-1);
    }

    heap = sbrk_heap(&pool->heap, 4 * WORD_SIZE);
    if (heap == (void *)-1) {
        return (-1);
    }
    PUT_WORD(heap, 0U);
    PUT_WORD(heap + (1 * WORD_SIZE), PACK(DWORD_SIZE, ALLOCATED));
    PUT_WORD(heap + (2 * WORD_SIZE), PACK(DWORD_SIZE, ALLOCATED));
    PUT_WORD(heap + (3 * WORD_SIZE), PACK(0, ALLOCATED));
    if (extend_pool(pool, (1 << 12) / WORD_SIZE) == NULL) {
        return (-1);
    }
    pool->beginning = NEXT_BLK(heap + (2 * WORD_SIZE));
    return (0);
}

void
print_pool(const t_pool *pool, bool show_free_blks) {
    void        *blk      = pool->beginning;
    t_free_list *free_blk = NULL;

    printf("## Pool [%lu;%lu] ##\n", pool->min_alloc_size, pool->max_alloc_size);
    while (GET_SIZE(GET_HDR(blk)) != 0) {
        if (GET_ALLOC(GET_HDR(blk)) == FREE && show_free_blks) {
            free_blk = FREE_LIST_ELEM(blk);
            if (blk == pool->head) {
                printf("Free Block %p : %u bytes [HEAD]\n", blk, GET_SIZE(GET_HDR(blk)));
            } else {
                printf("Free Block %p : %u bytes\n", blk, GET_SIZE(GET_HDR(blk)));
            }
            printf("\t- prev : %p\n", free_blk->prev);
            printf("\t- next : %p\n", free_blk->next);
        } else {
            printf("Allocated Block %p : %u bytes\n", blk, GET_SIZE(GET_HDR(blk)));
        }
        blk = NEXT_BLK(blk);
    }
    fflush(stdout);
}