/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 21:53:47 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/15 18:31:07 by plouvel          ###   ########.fr       */
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

void
release_pool(t_pool *pool) {
    if (pool->base_ptr) {
        (void)munmap(pool->base_ptr, pool->size);
        pool->beginning_ptr = NULL;
        pool->base_ptr      = NULL;
        pool->size          = 0;
    }
}

void *
find_fit_pool(const t_pool *pool, const size_t size) {
    void *blk = pool->beginning_ptr;

    while (GET_SIZE(GET_HDR(blk)) != 0) {
        if (GET_ALLOC(GET_HDR(blk)) == FREE && (size <= GET_SIZE(GET_HDR(blk)))) {
            return (blk);
        }
        blk = NEXT_BLK(blk);
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
init_fill_pool(t_byte *base_ptr, size_t free_blk_size) {
    void *epilogue_blk = NULL;
    void *free_blk     = NULL;
    void *prologue_blk = NULL;

    /* Alignement padding */

    PUT_WORD(base_ptr, 0x00000000U);

    /* Prologue block */

    prologue_blk = base_ptr + (2 * WORD_SIZE);

    PUT_WORD(GET_HDR(prologue_blk), PACK(DWORD_SIZE, ALLOCATED));
    PUT_WORD(GET_FTR(prologue_blk), PACK(DWORD_SIZE, ALLOCATED));

    /* Initial free block */

    free_blk = NEXT_BLK(prologue_blk);

    PUT_WORD(GET_HDR(free_blk), PACK(free_blk_size, FREE));
    PUT_WORD(GET_FTR(free_blk), PACK(free_blk_size, FREE));

    /* Epilogue block */

    epilogue_blk = NEXT_BLK(free_blk);

    PUT_WORD(GET_HDR(epilogue_blk), PACK(0, ALLOCATED));

    return (epilogue_blk);
}

int
init_pool(t_pool *pool) {
    pool->size     = get_adj_pool_size(pool->max_alloc_size);
    pool->base_ptr = mmap(NULL, pool->size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (pool->base_ptr == MAP_FAILED) {
        return (-1);
    }
    pool->beginning_ptr = init_fill_pool(pool->base_ptr, pool->size - (4 * WORD_SIZE));
    return (0);
}