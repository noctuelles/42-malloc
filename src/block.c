/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 22:41:34 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/22 17:07:13 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "block.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "utils.h"

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
    size_t new_free_blk_size  = 0;

    assert(xpnd_size % 8 == 0);

    if (GET_ALLOC(GET_HDR(next_blk)) == FREE) {
        if (next_free_blk_size >= xpnd_size) {
            new_free_blk_size = next_free_blk_size - xpnd_size;
            if (new_free_blk_size < MIN_BLK_SIZE) {
                delone_free_list(head, next_blk);

                PUT_WORD(GET_HDR(blk), PACK(blk_size + next_free_blk_size, ALLOCATED));
                PUT_WORD(GET_FTR(blk), PACK(blk_size + next_free_blk_size, ALLOCATED));
            } else {
                move_free_list_ptr(head, (t_free_list *)((t_byte *)next_blk + xpnd_size), next_blk);
                PUT_WORD(GET_HDR(blk), PACK(blk_size + xpnd_size, ALLOCATED));
                PUT_WORD(GET_FTR(blk), PACK(blk_size + xpnd_size, ALLOCATED));

                next_blk = NEXT_BLK(blk);

                PUT_WORD(GET_HDR(next_blk), PACK(new_free_blk_size, FREE));
                PUT_WORD(GET_FTR(next_blk), PACK(new_free_blk_size, FREE));
            }
            return (blk);
        }
    }
    return (NULL);
}

/**
 * @brief Try to shrink the block blk by shrk_size bytes. If the shrinking is not worth it, the block is left untouched.
 *
 * @param head head of the free list of the pool of blk.
 * @param blk block to be shrunk.
 * @param shrk_size double word aligned shrink size.
 * @return void* NULL if blk cannot be shrunk, blk otherwise.
 */
void *
shrink_blk(t_free_list **head, void *blk, size_t shrk_size) {
    size_t blk_size           = GET_SIZE(GET_HDR(blk));
    size_t new_blk_size       = blk_size - shrk_size;
    void  *created_free_block = NULL;

    assert(shrk_size % 8 == 0);

    if (new_blk_size < MIN_BLK_SIZE || shrk_size < MIN_BLK_SIZE) {
        return (NULL);
    }

    PUT_WORD(GET_HDR(blk), PACK(new_blk_size, ALLOCATED));
    PUT_WORD(GET_FTR(blk), PACK(new_blk_size, ALLOCATED));

    created_free_block = NEXT_BLK(blk);

    PUT_WORD(GET_HDR(created_free_block), PACK(shrk_size, FREE));
    PUT_WORD(GET_FTR(created_free_block), PACK(shrk_size, FREE));

    coalesce_blk(head, created_free_block);

    return (blk);
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

        move_free_list_ptr(head, blk, next_blk);
    } else if (!prev_blk_alloc && next_blk_alloc) {
        curr_blk_size += GET_SIZE(GET_HDR(prev_blk));

        PUT_WORD(GET_HDR(prev_blk), PACK(curr_blk_size, FREE));
        PUT_WORD(GET_FTR(blk), PACK(curr_blk_size, FREE));

        blk = prev_blk;
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

    if ((blk_size - adj_size) >= MIN_BLK_SIZE) {
        PUT_WORD(GET_HDR(blk), PACK(adj_size, ALLOCATED));
        PUT_WORD(GET_FTR(blk), PACK(adj_size, ALLOCATED));

        old_free_blk = blk;
        blk          = NEXT_BLK(blk);

        PUT_WORD(GET_HDR(blk), PACK(blk_size - adj_size, FREE));
        PUT_WORD(GET_FTR(blk), PACK(blk_size - adj_size, FREE));

        move_free_list_ptr(head, blk, old_free_blk);
    } else {
        PUT_WORD(GET_HDR(blk), PACK(blk_size, ALLOCATED));
        PUT_WORD(GET_FTR(blk), PACK(blk_size, ALLOCATED));

        delone_free_list(head, blk);
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
    size        = size + 2 * DWORD_SIZE;
    size        = align_on_page_size_boundary(size);
    t_byte *blk = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (blk == MAP_FAILED) {
        return (NULL);
    }

    /* Padding */
    PUT_WORD(blk + (0 * WORD_SIZE), 0x00000000U);
    /* Anonymous Block Size */
    PUT_DWORD(blk + (1 * WORD_SIZE), size);
    /* Header */
    PUT_WORD(blk + (1 * WORD_SIZE) + (1 * DWORD_SIZE), PACK(0, ALLOCATED | ANONYMOUS));

    return (blk + (2 * WORD_SIZE + 1 * DWORD_SIZE));
}

void
free_anonymous_blk(void *blk) {
    void *blk_header = GET_HDR(blk);

    munmap(GET_ANON_BASE(blk_header), GET_ANON_SIZE(blk_header));
}

static void
print_hdr_ftr(void *hdr) {
    printf("+-----------------------+\n");
    printf("|%3s%#010x%3s|    %c%c|\n", "", GET_SIZE(hdr), "", GET_ANONYMOUS(hdr) ? 'A' : 'N',
           GET_ALLOC(hdr) ? 'A' : 'F');
    printf("+-----------------------+\n");
}

static void
print_body(void *blk) {
    size_t  blk_size = GET_SIZE(GET_HDR(blk)) - 2 * WORD_SIZE;
    t_byte *pld_mem  = blk;
    size_t  i        = 0;
    size_t  j        = 0;

    if (GET_ALLOC(GET_HDR(blk)) == FREE) {
        printf("|prev : %16p|\n", FREE_LIST_ELEM(blk)->prev);
        printf("|next : %16p|\n", FREE_LIST_ELEM(blk)->next);
        blk_size -= 2 * DWORD_SIZE;
        pld_mem += 2 * DWORD_SIZE;
    }

    while (i < blk_size) {
        putchar('|');
        j = 0;
        while (j < DWORD_SIZE) {
            printf("%02x", pld_mem[i + j]);
            if (j != DWORD_SIZE - 1) {
                putchar(' ');
            }
            j++;
        }
        printf("|\n");
        i += DWORD_SIZE;
    }
}
/**
 * @brief Print the block information.
 *
 * @param blk block to be printed.
 */
void
print_blk(void *blk) {
    print_hdr_ftr(GET_HDR(blk));
    print_body(blk);
    print_hdr_ftr(GET_FTR(blk));
}