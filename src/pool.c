/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 21:53:47 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/28 13:59:53 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <assert.h>
#include <block.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "inc/malloc.h"
#include "malloc.h"
#include "malloc_utils.h"
#include "tunable.h"

/**
 * @brief Find a free block in the pool that can fit the adjusted size.
 *
 * @param pool The pool to search in.
 * @param adj_size The adjusted size to fit.
 * @return void* A pointer to the free block that can fit the adjusted size.
 */
void *
find_fit_in_pool(t_pool *pool, const size_t adj_size) {
    t_free_list *blk      = pool->head;
    size_t       blk_size = 0;

    while (blk != NULL) {
        blk_size = GET_SIZE(GET_HDR(blk));
        if (blk_size >= adj_size) {
            if ((blk_size - adj_size) >= MIN_BLK_SIZE) {
                return (blk);
            }
            if (blk_size <= pool->max_alloc_size) {
                return (blk);
            }
        }
        blk = blk->next;
    }
    return (NULL);
}

/**
 * @brief Find the pool that contains the block blk.
 *
 * @param pools Array of pools.
 * @param n Number of pools.
 * @param blk Block to find.
 * @return t_pool* The pool that contains the block blk.
 */
t_pool *
find_blk_in_pools(t_pool *pools, size_t n, void *blk) {
    size_t blk_size = GET_SIZE(GET_HDR(blk));
    size_t i        = 0;

    while (i < n) {
        if (blk_size >= pools[i].min_alloc_size && blk_size <= pools[i].max_alloc_size) {
            return (&pools[i]);
        }
        i++;
    }
    return (NULL);
}

/**
 * @brief Find a free block in the pools that can fit the adjusted size.
 *
 * @param pools Array of pools.
 * @param n Number of pools.
 * @param adj_size The adjusted size to fit.
 * @param blk_pool The pool that contains the free block.
 * @return void* NULL is no fit is found, a pointer to the free block otherwise.
 *
 * @note blk_pool is a result parameter. Do note that the function can return NULL, but blk_pool might be set if the
 * pool just needs an extension.
 * If blk_pool is NULL, then the size is too big to fit any pool, and needs an orphean
 * block.
 */
void *
find_fit_in_pools(t_pool *pools, size_t n, const size_t adj_size, t_pool **blk_pool) {
    size_t i = 0;

    while (i < n) {
        *blk_pool = &pools[i];
        if (adj_size >= (*blk_pool)->min_alloc_size && adj_size <= (*blk_pool)->max_alloc_size) {
            return (find_fit_in_pool(*blk_pool, adj_size));
        }
        i++;
    }
    *blk_pool = NULL;
    return (NULL);
}

void *
extend_pool(t_pool *pool, size_t words) {
    t_byte *heap_brk = NULL;
    void   *free_blk = NULL;
    size_t  bytes    = 0;

    bytes    = (words % 4) ? (words + 4 - (words % 4)) * WORD_SIZE : words * WORD_SIZE;
    heap_brk = sbrk_heap(&pool->heap, bytes);
    if (heap_brk == (void *)-1) {
        return (NULL);
    }
    PUT_WORD(GET_HDR(heap_brk), PACK(bytes, FREE));
    PUT_WORD(GET_FTR(heap_brk), PACK(bytes, FREE));
    PUT_WORD(GET_HDR(NEXT_BLK(heap_brk)), PACK(0, ALLOCATED));
    free_blk = coalesce_blk(&pool->head, heap_brk);
    return (free_blk);
}

/**
 * @brief Initialize a pool : set up the initial prologue and epilogue blocks.
 *
 * @param pool The pool to initialize.
 * @return int 0 on success, -1 on failure.
 */
int
init_pool(t_pool *pool) {
    t_byte *heap = NULL;

    heap = sbrk_heap(&pool->heap, 8 * WORD_SIZE);
    if (heap == (void *)-1) {
        return (-1);
    }
    /* Padding */
    PUT_WORD(heap + (0 * WORD_SIZE), 0x00000000U);
    PUT_WORD(heap + (1 * WORD_SIZE), 0x00000000U);
    PUT_WORD(heap + (2 * WORD_SIZE), 0x00000000U);
    /* Prologue */
    PUT_WORD(heap + (3 * WORD_SIZE), PACK(4 * WORD_SIZE, ALLOCATED));
    PUT_WORD(heap + (4 * WORD_SIZE), 0x00000000U);
    PUT_WORD(heap + (5 * WORD_SIZE), 0x00000000U);
    PUT_WORD(heap + (6 * WORD_SIZE), PACK(4 * WORD_SIZE, ALLOCATED));
    /* Epilogue */
    PUT_WORD(heap + (7 * WORD_SIZE), PACK(0, ALLOCATED));
    if (extend_pool(pool, get_tunable(FT_POOL_CHUNK_EXTENSION_STR, POOL_CHUNK_EXTENSION) / WORD_SIZE) == NULL) {
        return (-1);
    }
    pool->beginning = NEXT_BLK(heap + (4 * WORD_SIZE));
    return (0);
}

/**
 * @brief Print the pool.
 *
 * @param pool The pool to print.
 * @param opts Options to print.
 *
 * @note Available options are PRINT_FREE and PRINT_ALLOC.
 */
void
print_pool(const t_pool *pool, int opts) {
    void        *blk      = pool->beginning;
    t_free_list *free_blk = NULL;
    size_t       alloc    = 0;

    printf("## Pool [%lu;%lu] ##\n", pool->min_alloc_size, pool->max_alloc_size);
    while (GET_SIZE(GET_HDR(blk)) != 0) {
        alloc = GET_ALLOC(GET_HDR(blk));

        if (!alloc && opts & PRINT_FREE) {
            free_blk = FREE_LIST_ELEM(blk);
            if (blk == pool->head) {
                printf("\tFree Block %p : %u bytes [HEAD].\n", blk, GET_SIZE(GET_HDR(blk)));
            } else {
                printf("\tFree Block %p : %u bytes.\n", blk, GET_SIZE(GET_HDR(blk)));
            }
            printf("\t\t- prev : %p\n", free_blk->prev);
            printf("\t\t- next : %p\n", free_blk->next);
        } else if (alloc && opts & PRINT_ALLOC) {
            printf("\tAllocated Block %p : %u bytes, %lu usable.\n", blk, GET_SIZE(GET_HDR(blk)), GET_PLD_SIZE(blk));
        }
        blk = NEXT_BLK(blk);
    }
    fflush(stdout);
}

/**
 * @brief Print the pool in a pretty way, with an ASCII representation of the blocks.
 *
 * @param pool The pool to print.
 * @param opts Options to print.
 *
 * @note Available options are PRINT_FREE and PRINT_ALLOC.
 */
void
print_pretty_pool(const t_pool *pool, int opts) {
    void  *blk   = pool->beginning;
    size_t alloc = 0;

    printf("## Pool [%lu;%lu] ##\n\n", pool->min_alloc_size, pool->max_alloc_size);
    while (GET_SIZE(GET_HDR(blk)) != 0) {
        alloc = GET_ALLOC(GET_HDR(blk));
        if (alloc && (opts & PRINT_ALLOC)) {
            print_blk(blk);
        } else if (!alloc && (opts & PRINT_FREE)) {
            print_blk(blk);
        }
        blk = NEXT_BLK(blk);
    }
    fflush(stdout);
}