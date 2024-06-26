/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 21:53:47 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/04 13:33:25 by plouvel          ###   ########.fr       */
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
    t_list *blk      = pool->head;
    size_t  blk_size = 0;

    assert(pool->type == NORMAL_POOL);
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
    size_t blk_size = GET_ORPHEAN(GET_HDR(blk)) ? GET_ORPHEAN_SIZE(blk) : GET_SIZE(GET_HDR(blk));
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
 * @brief Find the appropriate pool for an allocation of size n.
 *
 * @param pools Array of pools.
 * @param n Number of pools.
 * @param size Size of the allocation.
 * @return t_pool* The appropriate pool for the allocation.
 *
 */
t_pool *
find_appropriate_pool_for_alloc(t_pool *pools, size_t n, size_t size) {
    size_t i = 0;

    while (i < n) {
        if (size >= pools[i].min_alloc_size && size <= pools[i].max_alloc_size) {
            if (pools[i].init == false) {
                if (init_pool(&pools[i]) == -1) {
                    return (NULL);
                }
                pools[i].init = true;
            }
            return (&pools[i]);
        }
        i++;
    }
    return (NULL);
}

/**
 * @brief
 *
 * @param pool
 * @param words
 * @return void* NULL on failure, a pointer to the free block otherwise.

 */
void *
extend_pool(t_pool *pool, size_t words) {
    t_byte *heap_brk = NULL;
    void   *free_blk = NULL;
    size_t  bytes    = 0;

    if (pool->type == ORPHEAN_POOL) {
        return (NULL);
    }
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

    if (pool->type == ORPHEAN_POOL) {
        return (0);
    }
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

/* Pool Printing */

static void
print_pool_orphean(const t_pool *pool, int opts) {
    t_list *elem = NULL;
    void   *blk  = NULL;

    fprintf(stderr, "## Orphean Pool [%lu;%lu] ##\n\n", pool->min_alloc_size, pool->max_alloc_size);
    fprintf(stderr, "\tNo heap allocated for this pool.\n\n");
    elem = pool->head;
    while (elem != NULL) {
        blk = (t_byte *)(elem) + sizeof(t_list) + WORD_SIZE;
        if (opts & PRINT_ALLOC) {
            fprintf(stderr, "\tAllocated Block %p : %lu bytes, %lu usable.\n", GET_ORPHEAN_BASE(blk),
                    GET_ORPHEAN_SIZE(blk), GET_ORPHEAN_SIZE(blk) - ORPHEAN_BLK_MISC_SIZE);
            if (opts & PRINT_ALLOC_HEXDUMP) {
                hexdump_blk_mem(stderr, "\t\t", blk);
            }
        }
        elem = elem->next;
    }
    fputc('\n', stderr);
}

static void
print_pool_normal(const t_pool *pool, int opts) {
    t_list *free_blk = NULL;
    void   *blk      = pool->beginning;
    size_t  alloc    = 0;

    fprintf(stderr, "## Pool [%lu;%lu] ##\n\n", pool->min_alloc_size, pool->max_alloc_size);
    fprintf(stderr, "\tHeap Reserved Address Range : [%p ; %p]\n", pool->heap.base, pool->heap.max_addr);
    fprintf(stderr, "\tUsed Address Range          : [%p ; %p]\n", pool->heap.base, pool->heap.brk);
    fprintf(stderr, "\tHeap Break                  : %p\n\n", pool->heap.brk);

    while (GET_SIZE(GET_HDR(blk)) != 0) {
        alloc = GET_ALLOC(GET_HDR(blk));

        if (!alloc && opts & PRINT_FREE) {
            free_blk = FREE_LIST_ELEM(blk);
            if (blk == pool->head) {
                fprintf(stderr, "\tFree Block %p : %u bytes [HEAD].\n", blk, GET_SIZE(GET_HDR(blk)));
            } else {
                fprintf(stderr, "\tFree Block %p : %u bytes.\n", blk, GET_SIZE(GET_HDR(blk)));
            }
            fprintf(stderr, "\t\t- prev : %p\n", free_blk->prev);
            fprintf(stderr, "\t\t- next : %p\n", free_blk->next);
            if (opts & PRINT_FREE_HEXDUMP) {
                hexdump_blk_mem(stderr, "\t\t", blk);
            }
        } else if (alloc && opts & PRINT_ALLOC) {
            fprintf(stderr, "\tAllocated Block %p : %u bytes, %lu usable.\n", blk, GET_SIZE(GET_HDR(blk)),
                    GET_SIZE(GET_HDR(blk)) - BLK_MISC_SIZE);
            if (opts & PRINT_ALLOC_HEXDUMP) {
                hexdump_blk_mem(stderr, "\t\t", blk);
            }
        }
        blk = NEXT_BLK(blk);
    }
    fputc('\n', stderr);
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
    if (pool->init == false) {
        return;
    }
    if (pool->type == ORPHEAN_POOL) {
        print_pool_orphean(pool, opts);
    } else {
        print_pool_normal(pool, opts);
    }
}
