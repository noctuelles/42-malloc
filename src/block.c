/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 22:41:34 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/15 17:31:35 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "block.h"

#include <assert.h>
#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>

/**
 * @brief Coalesce adjacent block if they are free.
 *
 * @param block_ptr
 * @return void*
 */
void *
coalesce_block(void *block_ptr) {
    void *next_block_ptr = NEXT_BLK(block_ptr);
    void *prev_block_ptr = PREV_BLK(block_ptr);

    size_t prev_block_allocated = GET_ALLOC(GET_FTR(prev_block_ptr));
    size_t next_block_allocated = GET_ALLOC(GET_HDR(next_block_ptr));
    size_t current_block_size   = GET_SIZE(GET_HDR(block_ptr));

    if (prev_block_allocated && next_block_allocated) {
        return (block_ptr);
    }
    if (prev_block_allocated && !next_block_allocated) {
        current_block_size += GET_SIZE(GET_HDR(next_block_ptr));

        PUT_WORD(GET_HDR(block_ptr), PACK(current_block_size, FREE));
        PUT_WORD(GET_FTR(block_ptr), PACK(current_block_size, FREE));

    } else if (!prev_block_allocated && next_block_allocated) {
        current_block_size += GET_SIZE(GET_HDR(prev_block_ptr));

        PUT_WORD(GET_HDR(prev_block_ptr), PACK(current_block_size, FREE));
        PUT_WORD(GET_FTR(block_ptr), PACK(current_block_size, FREE));

        block_ptr = prev_block_ptr;

    } else if (!prev_block_allocated && !next_block_allocated) {
        current_block_size += GET_SIZE(GET_HDR(prev_block_ptr));
        current_block_size += GET_SIZE(GET_HDR(next_block_ptr));

        PUT_WORD(GET_HDR(prev_block_ptr), PACK(current_block_size, FREE));
        PUT_WORD(GET_FTR(next_block_ptr), PACK(current_block_size, FREE));

        block_ptr = prev_block_ptr;
    }

    return (block_ptr);
}

void
place_block(void *blk, const size_t adj_size) {
    size_t blk_size = GET_SIZE(GET_HDR(blk));

    assert(blk_size >= adj_size);
    assert(GET_ALLOC(GET_HDR(blk)) == FREE);

    if ((blk_size - adj_size) > (2 * DWORD_SIZE)) {
        PUT_WORD(GET_HDR(blk), PACK(adj_size, ALLOCATED));
        PUT_WORD(GET_FTR(blk), PACK(adj_size, ALLOCATED));

        blk = NEXT_BLK(blk);

        PUT_WORD(GET_HDR(blk), PACK(blk_size - adj_size, FREE));
        PUT_WORD(GET_FTR(blk), PACK(blk_size - adj_size, FREE));

    } else {
        PUT_WORD(GET_HDR(blk), PACK(adj_size, ALLOCATED));
        PUT_WORD(GET_FTR(blk), PACK(adj_size, ALLOCATED));
    }
}

/**
 * @brief Allocate a new anonymous block. An anonymous block doesn't belong to any pool.
 *
 * @param size size of the block, aligned to a double word boundary.
 * @return void* pointer to the allocated bloc.
 */
void *
new_anonymous_block(size_t size) {
    const size_t page_size = (size_t)getpagesize();
    const size_t blk_size  = page_size * ((size + page_size) / page_size);
    t_byte      *blk       = mmap(NULL, blk_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (blk == MAP_FAILED) {
        return (NULL);
    }

    PUT_WORD(blk, 0x00000000U);
    PUT_DWORD(blk + (1 * WORD_SIZE), blk_size);
    PUT_WORD(blk + (1 * WORD_SIZE) + (1 * DWORD_SIZE), PACK(0, ALLOCATED | ANONYMOUS));

    return (blk + (2 * WORD_SIZE + 1 * DWORD_SIZE));
}