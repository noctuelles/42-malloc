/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 21:53:47 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/15 13:47:08 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pool.h"

#include <assert.h>
#include <block.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

void
release_pool(t_pool *pool) {
    if (pool->base_ptr) {
        (void)munmap(pool->base_ptr, pool->size);
        pool->beginning_ptr = NULL;
        pool->base_ptr      = NULL;
        pool->size          = 0;
    }
}

int
init_pool(t_pool *pool) {
    size_t free_blk_size = 0;

    pool->size     = POOL_ADJUSTED_SIZE(pool->max_alloc_size);
    free_blk_size  = pool->size - (4 * WORD_SIZE);
    pool->base_ptr = mmap(NULL, pool->size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (pool->base_ptr == MAP_FAILED) {
        return (-1);
    }

    /* Alignement padding */

    PUT_WORD(pool->base_ptr, 0x00000000U);

    /* Prologue block */

    PUT_WORD(pool->base_ptr + WORD_SIZE, PACK(DWORD_SIZE, ALLOCATED));
    PUT_WORD(pool->base_ptr + (2 * WORD_SIZE), PACK(DWORD_SIZE, ALLOCATED));

    /* Initial free block */

    PUT_WORD(pool->base_ptr + (3 * WORD_SIZE), PACK(free_blk_size, FREE));
    PUT_WORD(pool->base_ptr + (3 * WORD_SIZE) + free_blk_size - WORD_SIZE, PACK(free_blk_size, FREE));

    /* Epilogue block */

    PUT_WORD(pool->base_ptr + (3 * WORD_SIZE) + free_blk_size, PACK(0, ALLOCATED));

    pool->beginning_ptr = pool->base_ptr + (2 * WORD_SIZE);

    return (0);
}