/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   orphean_block.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 15:16:04 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/01 02:52:22 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <sys/mman.h>

#include "block.h"
#include "list.h"

/**
 * @brief Allocate a new anonymous block. An anonymous block doesn't belong to any pool.
 *
 * @param size size of the block, aligned to a quad word boundary.
 * @return void* pointer to the allocated bloc.
 */
void *
new_orphean_blk(t_list **head, size_t size) {
    t_byte *blk = mmap(NULL, align_on_page_size_boundary(size + ORPHEAN_BLK_MISC_SIZE), PROT_READ | PROT_WRITE,
                       MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (blk == MAP_FAILED) {
        return (NULL);
    }

    /* Padding */
    PUT_WORD(blk, 0x00000000U);
    /* Orphean Block Size */
    PUT_DWORD(blk + (1 * WORD_SIZE), size);
    /* List Element */
    PUT_DWORD(blk + (1 * WORD_SIZE) + (1 * DWORD_SIZE), -1);
    PUT_DWORD(blk + (1 * WORD_SIZE) + (2 * DWORD_SIZE), -1);
    /* Header */
    PUT_WORD(blk + (1 * WORD_SIZE) + (3 * DWORD_SIZE), PACK(0, ALLOCATED | ORPHEAN));

    blk += ORPHEAN_BLK_MISC_SIZE;

    push_front_list(head, GET_ORPHEAN_ELEM(blk));

    fflush(stdout);

    return (blk);
}

void
free_orphean_blk(t_list **head, void *ptr) {
    delone_list(head, GET_ORPHEAN_ELEM(ptr));
    munmap(GET_ORPHEAN_BASE(ptr), GET_ORPHEAN_SIZE(ptr));
}