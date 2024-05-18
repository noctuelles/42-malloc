/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 22:41:34 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/18 16:09:54 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "block.h"

#include <assert.h>
#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>

void
push_front_free_blk(void **free_list_head, void *free_blk) {
    assert(GET_ALLOC(GET_HDR(free_blk)) == FREE);

    if (*free_list_head == NULL) {
        PUT_DWORD(FREE_NEXT(free_blk), NULL);
    } else {
        PUT_DWORD(FREE_PREV(*free_list_head), free_blk);
        PUT_DWORD(FREE_NEXT(free_blk), *free_list_head);
    }
    PUT_DWORD(FREE_PREV(free_blk), NULL);
    *free_list_head = free_blk;
}

void
remove_free_blk(void **free_list_head, void *free_blk) {
    assert(GET_ALLOC(GET_HDR(free_blk)) == FREE);

    if (*free_list_head = free_blk) {
        *free_list_head = GET_DWORD(FREE_NEXT(free_blk));

        if (*free_list_head) {
            PUT_DWORD(FREE_PREV(*free_list_head), NULL);
        }
    } else {
        PUT_DWORD(FREE_NEXT(GET_DWORD(FREE_PREV(free_blk))), FREE_NEXT(free_blk));

        if (GET_DWORD(FREE_NEXT(free_blk))) {
            PUT_DWORD(FREE_PREV(GET_DWORD(FREE_NEXT(free_blk))), FREE_PREV(free_blk));
        }
    }
}

void
move_blk_list_values(void *free_blk, void *old_free_blk) {
    PUT_DWORD(FREE_PREV(free_blk), FREE_PREV(old_free_blk));
    PUT_DWORD(FREE_NEXT(free_blk), FREE_NEXT(old_free_blk));

    if (GET_DWORD(FREE_PREV(free_blk)) != NULL) {
        PUT_DWORD(FREE_NEXT(GET_DWORD(FREE_PREV(free_blk))), free_blk);
    }

    if (GET_DWORD(FREE_PREV(free_blk)) != NULL) {
        PUT_DWORD(FREE_PREV(GET_DWORD(FREE_NEXT(free_blk))), free_blk);
    }
}

/**
 * @brief Coalesce adjacent block if they are free.
 *
 * @param block_ptr
 * @return void*
 */
void *
coalesce_block(void **free_list_head, void *blk_ptr) {
    void *next_blk_ptr = NEXT_BLK(blk_ptr);
    void *prev_blk_ptr = PREV_BLK(blk_ptr);

    size_t prev_blk_allocated = GET_ALLOC(GET_FTR(prev_blk_ptr));
    size_t next_blk_allocated = GET_ALLOC(GET_HDR(next_blk_ptr));
    size_t curr_blk_size      = GET_SIZE(GET_HDR(blk_ptr));

    if (prev_blk_allocated && next_blk_allocated) {
        push_front_free_blk(free_list_head, blk_ptr);

        return (blk_ptr);
    }
    if (prev_blk_allocated && !next_blk_allocated) {
        curr_blk_size += GET_SIZE(GET_HDR(next_blk_ptr));

        PUT_WORD(GET_HDR(blk_ptr), PACK(curr_blk_size, FREE));
        PUT_WORD(GET_FTR(blk_ptr), PACK(curr_blk_size, FREE));
    } else if (!prev_blk_allocated && next_blk_allocated) {
        curr_blk_size += GET_SIZE(GET_HDR(prev_blk_ptr));

        PUT_WORD(GET_HDR(prev_blk_ptr), PACK(curr_blk_size, FREE));
        PUT_WORD(GET_FTR(blk_ptr), PACK(curr_blk_size, FREE));

        blk_ptr = prev_blk_ptr;

        move_blk_list_values(blk_ptr, next_blk_ptr);
    } else if (!prev_blk_allocated && !next_blk_allocated) {
        curr_blk_size += GET_SIZE(GET_HDR(prev_blk_ptr)) + GET_SIZE(GET_HDR(next_blk_ptr));

        PUT_WORD(GET_HDR(prev_blk_ptr), PACK(curr_blk_size, FREE));
        PUT_WORD(GET_FTR(next_blk_ptr), PACK(curr_blk_size, FREE));

        blk_ptr = prev_blk_ptr;

        remove_free_blk(free_list_head, next_blk_ptr);
    }

    return (blk_ptr);
}

void
place_block(void **free_list_head, void *blk, const size_t adj_size) {
    size_t blk_size = GET_SIZE(GET_HDR(blk));

    assert(blk_size >= adj_size);
    assert(GET_ALLOC(GET_HDR(blk)) == FREE);

    if ((blk_size - adj_size) > (4 * DWORD_SIZE)) {
        PUT_WORD(GET_HDR(blk), PACK(adj_size, ALLOCATED));
        PUT_WORD(GET_FTR(blk), PACK(adj_size, ALLOCATED));

        blk = NEXT_BLK(blk);

        PUT_WORD(GET_HDR(blk), PACK(blk_size - adj_size, FREE));
        PUT_WORD(GET_FTR(blk), PACK(blk_size - adj_size, FREE));

        push_front_free_blk(free_list_head, blk);
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