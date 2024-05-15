/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 22:41:34 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/15 12:31:49 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "block.h"

#include <stddef.h>

/**
 * @brief Coalesce adjacent block if they are free.
 *
 * @param block_ptr
 * @return void*
 */
void *
block_coalesce(void *block_ptr) {
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