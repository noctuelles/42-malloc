/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 22:41:34 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/13 12:25:51 by plouvel          ###   ########.fr       */
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
    void *next_block_ptr = NEXT_BLOCK_PTR(block_ptr);
    void *prev_block_ptr = PREV_BLOCK_PTR(block_ptr);

    size_t prev_block_allocated = IS_ALLOCATED(GET_FOOTER(prev_block_ptr));
    size_t next_block_allocated = IS_ALLOCATED(GET_HEADER(next_block_ptr));
    size_t current_block_size   = GET_SIZE(GET_HEADER(block_ptr));

    if (prev_block_allocated && next_block_allocated) {
        return (block_ptr);
    }

    if (prev_block_allocated && !next_block_allocated) {
        current_block_size += GET_SIZE(GET_HEADER(next_block_ptr));

        PUT_WORD(GET_HEADER(block_ptr), PACK_HEADER_FOOTER(current_block_size, FREE));
        PUT_WORD(GET_FOOTER(block_ptr), PACK_HEADER_FOOTER(current_block_size, FREE));

    } else if (!prev_block_allocated && next_block_allocated) {
        current_block_size += GET_SIZE(GET_HEADER(prev_block_ptr));

        PUT_WORD(GET_HEADER(prev_block_ptr), PACK_HEADER_FOOTER(current_block_size, FREE));
        PUT_WORD(GET_FOOTER(block_ptr), PACK_HEADER_FOOTER(current_block_size, FREE));

        block_ptr = prev_block_ptr;

    } else if (!prev_block_allocated && !next_block_allocated) {
        current_block_size += GET_SIZE(GET_HEADER(prev_block_ptr));
        current_block_size += GET_SIZE(GET_HEADER(next_block_ptr));

        PUT_WORD(GET_HEADER(prev_block_ptr), PACK_HEADER_FOOTER(current_block_size, FREE));
        PUT_WORD(GET_FOOTER(next_block_ptr), PACK_HEADER_FOOTER(current_block_size, FREE));

        block_ptr = prev_block_ptr;
    }

    return (block_ptr);
}