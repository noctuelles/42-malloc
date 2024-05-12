/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 16:48:33 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/12 14:31:27 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLOCK_H
#define BLOCK_H

#include "stdint.h"

/*

Structure of the implicit free list :

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
            +--------------+

    N.B: (Padding) means it is optionnal.
*/

typedef uint8_t  t_byte;
typedef uint32_t t_word;
typedef uint64_t t_dword;

#define WORD_SIZE sizeof(t_word)
#define DWORD_SIZE sizeof(t_dword)

#define GET_WORD(ptr) (*(t_word *)(ptr))
#define PUT_WORD(ptr, val) (*(t_word *)(ptr) = (val))

/* The first 3 LSB are unused : the block size must be a multiple of 8 bytes
   (for aligment). The size is thus encoded in the remaining 29 bits.
 */
#define GET_BLOCK_SIZE(hdr_or_foot_ptr) (GET_WORD(hdr_or_foot_ptr) & ~0b111)
#define IS_ALLOCATED_BLOCK(hdr_or_foot_ptr) (GET_WORD(hdr_or_foot_ptr) & 0x1)

#define PACK_HEADER_FOOTER(size, is_allocated) ((size) | (is_allocated))

#define GET_HEADER(payload_ptr) ((t_byte *)(payload_ptr) - WORD_SIZE)
/**
 * @brief Compute the footer of the block given a payload pointer.
 * @note Minus DWORD_SIZE : the footer and header are included in the block
 * size, thus, we've to substract a DWORD.
 */
#define GET_FOOTER(payload_ptr) ((t_byte *)(payload_ptr) + GET_BLOCK_SIZE(GET_HEADER(payload_ptr)) - DWORD_SIZE)

/**
 * @brief Compute the block pointer of the next block.
 * @note To get to he next block pointer, we have to read the block size from the header, and increment accordingly.
 */
#define NEXT_BLOCK_PTR(payload_ptr) ((t_byte *)(payload_ptr) + GET_BLOCK_SIZE(((t_byte *)(payload_ptr) - WORD_SIZE)))
/**
 * @brief Compute the block pointer of the previous block.
 * @note To get to he previous block pointer, we have to read the previous block size footer, and decrement accordingly.
 */
#define PREV_BLOCK_PTR(payload_ptr) ((t_byte *)(payload_ptr) - GET_BLOCK_SIZE(((t_byte *)(payload_ptr) - DWORD_SIZE)))

#endif