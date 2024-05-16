/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 16:48:33 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/16 22:16:27 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLOCK_H
#define BLOCK_H

#include "malloc.h"
#include "stdint.h"

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

#define WORD_SIZE sizeof(t_word)
#define DWORD_SIZE sizeof(t_dword)

#define ALLOCATED 1U
#define ANONYMOUS (1U << 1)
#define FREE 0U

#define GET_WORD(ptr) (*(t_word *)(ptr))
#define GET_DWORD(ptr) (*(t_dword *)(ptr))
#define PUT_WORD(ptr, val) (*(t_word *)(ptr) = (val))

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

void *new_anonymous_block(size_t size);
void *coalesce_block(void *block_ptr);
void  place_block(void *blk, const size_t adj_size);

#endif