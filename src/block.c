/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 22:41:34 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/20 17:59:59 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "block.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "utils.h"

void
push_front_free_list(t_free_list **head, t_free_list *free_blk) {
    if (*head == NULL) {
        free_blk->next = NULL;
    } else {
        (*head)->prev  = free_blk;
        free_blk->next = *head;
    }
    free_blk->prev = NULL;
    *head          = free_blk;
}

void
delone_free_list(t_free_list **head, t_free_list *free_blk) {
    if (*head == free_blk) {
        *head = free_blk->next;

        if (*head) {
            (*head)->prev = NULL;
        }
    } else {
        free_blk->prev->next = free_blk->next;

        if (free_blk->next) {
            free_blk->next->prev = free_blk->prev;
        }
    }
}

void
move_free_list_values(t_free_list **head, t_free_list *free_blk, t_free_list *old_free_blk) {
    free_blk->prev = old_free_blk->prev;
    free_blk->next = old_free_blk->next;

    if (old_free_blk->prev != NULL) {
        old_free_blk->prev->next = free_blk;
    }
    if (old_free_blk->next != NULL) {
        old_free_blk->next->prev = free_blk;
    }
    if (old_free_blk == *head) {
        *head = free_blk;
    }
}

/**
 * @brief Tries to expand the block blk by xpnd_size bytes by using the next adjacent free block.
 *
 * @param head head of the free list of the pool of blk.
 * @param blk the block expansion block.
 * @param xpnd_size double word aligned expand size.
 * @return void* NULL if blk cannot be expanded, blk otherwise.
 */
void *
expand_blk(t_free_list **head, void *blk, size_t xpnd_size) {
    void  *next_blk           = NEXT_BLK(blk);
    size_t blk_size           = GET_SIZE(GET_HDR(blk));
    size_t next_free_blk_size = GET_SIZE(GET_HDR(next_blk));
    size_t size_delta         = 0;

    assert(xpnd_size % 8 == 0);

    if (GET_ALLOC(GET_HDR(next_blk)) == FREE) {
        if (next_free_blk_size >= xpnd_size) {
            size_delta = next_free_blk_size - xpnd_size;
            if (size_delta < MIN_BLK_SIZE) {
                delone_free_list(head, next_blk);

                PUT_WORD(GET_HDR(blk), PACK(blk_size + next_free_blk_size, ALLOCATED));
                PUT_WORD(GET_FTR(blk), PACK(blk_size + next_free_blk_size, ALLOCATED));
            } else {
                move_free_list_values(head, (t_free_list *)((t_byte *)next_blk + xpnd_size), next_blk);
                PUT_WORD(GET_HDR(blk), PACK(blk_size + xpnd_size, ALLOCATED));
                PUT_WORD(GET_FTR(blk), PACK(blk_size + xpnd_size, ALLOCATED));

                next_blk = NEXT_BLK(blk);

                PUT_WORD(GET_HDR(next_blk), PACK(next_free_blk_size - xpnd_size, FREE));
                PUT_WORD(GET_FTR(next_blk), PACK(next_free_blk_size - xpnd_size, FREE));
            }
            return (blk);
        }
    }
    return (NULL);
}

/**
 * @brief Coalesce adjacent free block around blk.
 *
 * @param head head of the free list of the pool of blk.
 * @param blk free block to be coalesced.
 * @return void* the free block resulting from the coalescing.
 */
void *
coalesce_blk(t_free_list **head, void *blk) {
    void *next_blk = NEXT_BLK(blk);
    void *prev_blk = PREV_BLK(blk);

    size_t prev_blk_alloc = GET_ALLOC(GET_FTR(prev_blk));
    size_t next_blk_alloc = GET_ALLOC(GET_HDR(next_blk));
    size_t curr_blk_size  = GET_SIZE(GET_HDR(blk));

    if (prev_blk_alloc && next_blk_alloc) {
        push_front_free_list(head, blk);

        return (blk);
    }
    if (prev_blk_alloc && !next_blk_alloc) {
        curr_blk_size += GET_SIZE(GET_HDR(next_blk));

        PUT_WORD(GET_HDR(blk), PACK(curr_blk_size, FREE));
        PUT_WORD(GET_FTR(blk), PACK(curr_blk_size, FREE));

        move_free_list_values(head, blk, next_blk);
    } else if (!prev_blk_alloc && next_blk_alloc) {
        curr_blk_size += GET_SIZE(GET_HDR(prev_blk));

        PUT_WORD(GET_HDR(prev_blk), PACK(curr_blk_size, FREE));
        PUT_WORD(GET_FTR(blk), PACK(curr_blk_size, FREE));
    } else if (!prev_blk_alloc && !next_blk_alloc) {
        curr_blk_size += GET_SIZE(GET_HDR(prev_blk)) + GET_SIZE(GET_HDR(next_blk));

        PUT_WORD(GET_HDR(prev_blk), PACK(curr_blk_size, FREE));
        PUT_WORD(GET_FTR(next_blk), PACK(curr_blk_size, FREE));

        blk = prev_blk;

        delone_free_list(head, next_blk);
    }

    return (blk);
}

void
place_blk(t_free_list **head, void *blk, const size_t adj_size) {
    size_t blk_size     = GET_SIZE(GET_HDR(blk));
    void  *old_free_blk = NULL;

    assert(blk_size >= adj_size);
    assert(GET_ALLOC(GET_HDR(blk)) == FREE);

    if ((blk_size - adj_size) > (4 * DWORD_SIZE)) {
        PUT_WORD(GET_HDR(blk), PACK(adj_size, ALLOCATED));
        PUT_WORD(GET_FTR(blk), PACK(adj_size, ALLOCATED));

        old_free_blk = blk;
        blk          = NEXT_BLK(blk);

        PUT_WORD(GET_HDR(blk), PACK(blk_size - adj_size, FREE));
        PUT_WORD(GET_FTR(blk), PACK(blk_size - adj_size, FREE));

        move_free_list_values(head, blk, old_free_blk);
#ifndef NDEBUG
        bzero(old_free_blk, sizeof(t_free_list));
#endif
    } else {
        PUT_WORD(GET_HDR(blk), PACK(adj_size, ALLOCATED));
        PUT_WORD(GET_FTR(blk), PACK(adj_size, ALLOCATED));
#ifndef NDEBUG
        bzero(blk, sizeof(t_free_list));
#endif
    }
}

/**
 * @brief Allocate a new anonymous block. An anonymous block doesn't belong to any pool.
 *
 * @param size size of the block, aligned to a double word boundary.
 * @return void* pointer to the allocated bloc.
 */
void *
new_anonymous_blk(size_t size) {
    size        = align_on_page_size_boundary(size);
    t_byte *blk = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (blk == MAP_FAILED) {
        return (NULL);
    }

    PUT_WORD(blk, 0U);
    PUT_DWORD(blk + (1 * WORD_SIZE), size);
    PUT_WORD(blk + (1 * WORD_SIZE) + (1 * DWORD_SIZE), PACK(0, ALLOCATED | ANONYMOUS));

    return (blk + (2 * WORD_SIZE + 1 * DWORD_SIZE));
}