/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 21:53:47 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/14 15:38:02 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pool.h"

#include <block.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

size_t
compute_pool_size(t_pool *pool) {
    const size_t page_size = (size_t)getpagesize();
    const size_t size      = pool->max_alloc_size * ALLOC_PER_POOL + (WORD_SIZE * 4);

    pool->size = size + (page_size - size % page_size);
}

void
release_pool(t_pool *pool) {
    (void)munmap(pool->base_ptr, pool->size);
    pool->beginning_ptr = NULL;
    pool->size          = 0;
}

int
init_pool(t_pool *pool) {
    pool->size     = POOL_ADJUSTED_SIZE(pool->max_alloc_size);
    pool->base_ptr = mmap(NULL, pool->size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (pool->beginning_ptr == MAP_FAILED) {
        return (-1);
    }

    /* Alignement padding */

    PUT_WORD(pool->base_ptr, 0x00000000U);

    /* Prologue block */

    PUT_WORD(pool->base_ptr + WORD_SIZE, PACK_HEADER_FOOTER(DWORD_SIZE, ALLOCATED));
    PUT_WORD(pool->base_ptr + (2 * WORD_SIZE), PACK_HEADER_FOOTER(DWORD_SIZE, ALLOCATED));

    /* Epilogue block */

    PUT_WORD(pool->base_ptr + (3 * WORD_SIZE), PACK_HEADER_FOOTER(0, ALLOCATED));

    pool->beginning_ptr = pool->base_ptr + (2 * WORD_SIZE);

    return (0);
}