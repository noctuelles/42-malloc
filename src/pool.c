/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 21:53:47 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/14 21:04:12 by plouvel          ###   ########.fr       */
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

/**
 * @brief find an appropriate free block for the requested size. If no free block is available, it returns NULL.
 *
 * @param size allocation size.
 * @return void* the free block or NULL.
 */
void *
find_pool_free_block(t_pool *pool, size_t size) {
    void  *blk        = pool->beginning_ptr;
    t_word block_size = 0;

    while ((block_size = GET_SIZE(GET_HEADER(blk))) != 0) {
        if (block_size >= size) {
            return (blk);
        }
        blk = NEXT_BLOCK_PTR(blk);
    }
    return (NULL);
}

// void
// place_pool_block(const t_pool *pool, void *blk, size_t alloc_size) {
//     uint32_t blk_size = GET_SIZE(GET_HEADER(blk));

//     assert(IS_ALLOCATED(GET_HEADER(blk)) == FREE);
//     assert(blk_size >= (uint32_t)alloc_size);

//     if (blk_size - DWORD_SIZE > alloc_size) {
//         PUT_WORD(GET_HEADER(blk), PACK_HEADER_FOOTER(blk_size - alloc_size, ALLOCATED));
//         PUT_WORD(GET_FOOTER(blk), PACK_HEADER_FOOTER(blk_size - alloc_size, ALLOCATED));

//         PUT_WORD(GET_HEADER(NEXT_BLOCK_PTR(blk)), PACK_HEADER_FOOTER(blk_size - alloc_size, FREE));

//     } else {
//         PUT_WORD(GET_HEADER(blk), PACK_HEADER_FOOTER(blk_size, ALLOCATED));
//         PUT_WORD(GET_FOOTER(blk), PACK_HEADER_FOOTER(blk_size, ALLOCATED));
//     }
// }

void
show_pool(const t_pool *pool) {
    void  *blk      = NULL;
    t_word blk_size = 0;

    printf("Pool size [%lu;%lu]", pool->min_alloc_size, pool->max_alloc_size);
    if (pool->base_ptr == NULL) {
        printf(" - not yet initialized.\n");
        return;
    }
    printf(" - Starting at address %p.\n", pool->base_ptr);

    blk = pool->beginning_ptr;
    while ((blk_size = GET_SIZE(GET_HEADER(blk))) != 0) {
        printf("%p - %p : %lu", blk, GET_FOOTER(blk), blk_size - DWORD_SIZE);

        if (IS_ALLOCATED(GET_HEADER(blk))) {
            printf(" allocated byte(s).\n");
        } else {
            printf(" free byte(s).\n");
        }
        blk = NEXT_BLOCK_PTR(blk);
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

    PUT_WORD(pool->base_ptr + WORD_SIZE, PACK_HEADER_FOOTER(DWORD_SIZE, ALLOCATED));
    PUT_WORD(pool->base_ptr + (2 * WORD_SIZE), PACK_HEADER_FOOTER(DWORD_SIZE, ALLOCATED));

    /* Initial free block */

    PUT_WORD(pool->base_ptr + (3 * WORD_SIZE), PACK_HEADER_FOOTER(free_blk_size, FREE));
    PUT_WORD(pool->base_ptr + (3 * WORD_SIZE) + free_blk_size - WORD_SIZE, PACK_HEADER_FOOTER(free_blk_size, FREE));

    /* Epilogue block */

    PUT_WORD(pool->base_ptr + (3 * WORD_SIZE) + free_blk_size, PACK_HEADER_FOOTER(0, ALLOCATED));

    pool->beginning_ptr = pool->base_ptr + (2 * WORD_SIZE);

    return (0);
}