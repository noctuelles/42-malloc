/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 16:48:33 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/20 17:09:17 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>

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

typedef struct s_free_list {
    struct s_free_list *prev;
    struct s_free_list *next;
} t_free_list;

#define ALLOCATED 1U
#define ANONYMOUS (1U << 1)
#define FREE 0U
#define MIN_BLK_SIZE (4 * DWORD_SIZE)

#define GET_WORD(ptr) (*(t_word *)(ptr))
#define GET_DWORD(ptr) (*(t_dword *)(ptr))
#define PUT_WORD(ptr, val) (*(t_word *)(ptr) = (val))
#define PUT_DWORD(ptr, val) (*(t_dword *)(ptr) = (val))

/* The first 3 LSB are unused : the block size must be a multiple of 8 bytes
   (for aligment). The size is thus encoded in the remaining 29 bits.
 */
#define GET_SIZE(hdr_or_ftr) (GET_WORD(hdr_or_ftr) & ~0b111)
#define GET_ALLOC(hdr_or_ftr) (GET_WORD(hdr_or_ftr) & ALLOCATED)
#define GET_ANONYMOUS(hdr_or_ftr) (GET_WORD(hdr_or_ftr) & ANONYMOUS)
#define GET_PAYLOAD_SIZE(hdr_or_ftr) (GET_SIZE(hdr_or_ftr) - (2 * WORD_SIZE))

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

#define FREE_LIST_ELEM(blk_ptr) ((t_free_list *)(blk_ptr))

void *new_anonymous_blk(size_t size);

void move_free_list_values(t_free_list **head, t_free_list *free_blk, t_free_list *old_free_blk);
void push_front_free_list(t_free_list **head, t_free_list *free_blk);
void delone_free_list(t_free_list **head, t_free_list *free_blk);

void *expand_blk(t_free_list **head, void *blk, size_t xpnd_size);
void *coalesce_blk(t_free_list **free_list_head, void *blk);
void  place_blk(t_free_list **head, void *blk, const size_t adj_size);

#endif