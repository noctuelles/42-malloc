/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 21:53:47 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/12 22:40:15 by plouvel          ###   ########.fr       */
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

int
init_pool(t_pool *pool) {
    pool->size = compute_pool_size(&pool);
    pool->ptr  = mmap(NULL, pool->size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (pool->ptr) {
        error(0, errno, "cannot allocate heap size");
        return (-1);
    }

    /* Alignement */

    PUT_WORD(pool->ptr, 0);

    /* Prologue block */

    PUT_WORD(pool->ptr + WORD_SIZE, PACK_HEADER_FOOTER(DWORD_SIZE, ALLOCATED));
    PUT_WORD(pool->ptr + (2 * WORD_SIZE), PACK_HEADER_FOOTER(DWORD_SIZE, ALLOCATED));

    /* Epilogue block */

    PUT_WORD(pool->ptr + (3 * WORD_SIZE), PACK_HEADER_FOOTER(0, ALLOCATED));

    pool->ptr += (2 * WORD_SIZE);

    return (0);
}