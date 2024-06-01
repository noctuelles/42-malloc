/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 16:48:33 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/01 02:44:38 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLOCK_H
#define BLOCK_H

#include <stdbool.h>
#include <stdint.h>

#include "list.h"
#include "utils.h"

/*
Structure of the implicit free list :

        Header structure and footer structure :

        00000101000010000010000001000    001
        +---------------------------+---+---+
        |    Block Size (29 bits)   |   |UNA|
        +---------------------------+---+---+
        |           32 bits                 +
        +-----------------------------------+

        Last three bits :

                - U : Unused
                - N : Anonymous -> if the block is not part of any pool, the bit is set.
                - A : Allocated -> if the block is allocated, the bit is set.

        When an anonymous block is freed, munmap is directly called.

        The payload address is 8 bytes aligned. Thus, any type can start on this address.

        Example of the two continous block in a pool :

Low addr    +--------------+
            |    Header    |
    ========+--------------+======== Double word aligned
            |              |
            |              |
            |              |
            |    Payload   |
            |              |
            |              |
            |              |
            +--------------+
            |   (Padding)  |
            +--------------+
            |    Footer    |
            +--------------+
            |    Header    |
    ========+--------------+======== Double word aligned
            |              |
            |              |
            |              |
            |    Payload   |
            |              |
            |              |
            |              |
            +--------------+
            |   (Padding)  |
            +--------------+
            |    Footer    |
High addr   +--------------+

    N.B: (Padding) means it is optionnal.
*/

#define ALLOCATED 1U
#define ORPHEAN (1U << 1)
#define FREE 0U

#define BLK_MISC_SIZE (2 * WORD_SIZE)
#define ORPHEAN_BLK_MISC_SIZE (4 * DWORD_SIZE)

#define BLK_FREE_LIST_ELEM_SIZE (2 * DWORD_SIZE)

#define MIN_BLK_SIZE (4 * DWORD_SIZE)

#define GET_WORD(ptr) (*(t_word *)(ptr))
#define GET_DWORD(ptr) (*(t_dword *)(ptr))
#define PUT_WORD(ptr, val) (*(t_word *)(ptr) = (val))
#define PUT_DWORD(ptr, val) (*(t_dword *)(ptr) = (val))

/* The first 4 LSB are unused : the block size must be a multiple of 16 bytes
   (for aligment). The size is thus encoded in the remaining 28 bits.
 */
#define GET_SIZE(hdr_or_ftr) (GET_WORD(hdr_or_ftr) & ~0b1111)
#define GET_ALLOC(hdr_or_ftr) (GET_WORD(hdr_or_ftr) & ALLOCATED)
#define GET_PAYLOAD_SIZE(hdr_or_ftr) (GET_SIZE(hdr_or_ftr) - (2 * WORD_SIZE))

#define GET_ORPHEAN(hdr_or_ftr) (GET_WORD(hdr_or_ftr) & ORPHEAN)
#define GET_ORPHEAN_BASE(hdr) ((t_byte *)(hdr) - (4 * DWORD_SIZE))
#define GET_ORPHEAN_SIZE(hdr) (*(size_t *)(GET_ORPHEAN_BASE(hdr) + (1 * WORD_SIZE)))
#define GET_ORPHEAN_ELEM(hdr) ((t_list *)(GET_ORPHEAN_BASE(hdr) + (1 * WORD_SIZE) + (1 * DWORD_SIZE)))

#define PACK(size, is_allocated) ((size) | (is_allocated))

#define GET_HDR(payload_ptr) ((t_byte *)(payload_ptr) - WORD_SIZE)
/**
 * @brief Compute the footer of the block given a payload pointer.
 * @note Minus DWORD_SIZE : the footer and header are included in the block
 * size, thus, we've to substract a DWORD.
 */
#define GET_FTR(payload_ptr) ((t_byte *)(payload_ptr) + GET_SIZE(GET_HDR(payload_ptr)) - DWORD_SIZE)

/**
 * @brief Compute the block pointer of the next block.
 * @note To get to he next block pointer, we have to read the block size from the header, and increment accordingly.
 */
#define NEXT_BLK(payload_ptr) ((t_byte *)(payload_ptr) + GET_SIZE(((t_byte *)(payload_ptr) - WORD_SIZE)))
/**
 * @brief Compute the block pointer of the previous block.
 * @note To get to he previous block pointer, we have to read the previous block size footer, and decrement accordingly.
 */
#define PREV_BLK(payload_ptr) ((t_byte *)(payload_ptr) - GET_SIZE(((t_byte *)(payload_ptr) - DWORD_SIZE)))

#define GET_PLD_SIZE(blk) ((ptrdiff_t)(GET_FTR(blk) - (t_byte *)(blk)))

#define FREE_LIST_ELEM(blk_ptr) ((t_list *)(blk_ptr))

void *new_orphean_blk(t_list **head, size_t size);
void  free_orphean_blk(t_list **head, void *ptr);

bool  can_expand_blk(void *blk, size_t xpnd_size, size_t pool_max_alloc_size);
void *expand_blk(t_list **head, void *blk, size_t xpnd_size);

bool  can_shrink_blk(void *blk, size_t shrk_size, size_t pool_min_alloc_size);
void *shrink_blk(t_list **head, void *blk, size_t shrk_size);

void *coalesce_blk(t_list **head, void *blk);
void  place_blk(t_list **head, void *blk, const size_t adj_size);

void print_blk(void *blk);

#endif