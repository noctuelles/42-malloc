/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   orphean_block.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 15:16:04 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/30 15:35:14 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>

#include "block.h"

/**
 * @brief Fill an anonymous block with the appropriate header and footer.
 *
 * @param blk pointer to the block.
 * @param size size of the block.
 * @return void* pointer to the payload.
 */
void *
fill_orphean_blk(uint8_t *blk, size_t size) {
    /* Padding */
    PUT_WORD(blk + (0 * WORD_SIZE), 0x00000000U);
    /* Anonymous Block Size */
    PUT_DWORD(blk + (1 * WORD_SIZE), size);
    /* Header */
    PUT_WORD(blk + (1 * WORD_SIZE) + (1 * DWORD_SIZE), PACK(0, ALLOCATED | ORPHEAN));

    return (blk + (2 * WORD_SIZE + 1 * DWORD_SIZE));
}

/**
 * @brief Allocate a new anonymous block. An anonymous block doesn't belong to any pool.
 *
 * @param size size of the block, aligned to a quad word boundary.
 * @return void* pointer to the allocated bloc.
 */
void *
new_orphean_blk(size_t size) {
    size        = size + ORPHEAN_BLK_MISC_SIZE;
    size        = align_on_page_size_boundary(size);
    t_byte *blk = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (blk == MAP_FAILED) {
        return (NULL);
    }
    return (fill_orphean_blk(blk, size));
}

void
free_orphean_blk(void *ptr) {
    void *blk_hdr = GET_HDR(ptr);

    munmap(GET_ORPHEAN_BASE(blk_hdr), GET_ORPHEAN_SIZE(blk_hdr));
}