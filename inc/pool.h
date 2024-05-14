/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 13:46:14 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/14 20:56:35 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POOL_H
#define POOL_H

#include <stddef.h>
#include <stdint.h>

#define N_POOLS 2
#define ALLOC_PER_POOL 100

#define POOL_ONE_MIN_ALLOC_SIZE 1
#define POOL_ONE_MAX_ALLOC_SIZE 512

#define POOL_TWO_MIN_ALLOC_SIZE (POOL_ONE_MAX_ALLOC_SIZE + 1)
#define POOL_TWO_MAX_ALLOC_SIZE 4096

/**
 * @note addition of 4 words : one for padding, two for epilogue header and footer, and one for prologue header.
 * According to the subject, a pool must contains at least ALLOC_PER_POOL allocations. Given a pool size class of [n,
 * m], i choosed to allocate a pool of m * ALLOC_PER_POOL.
 */
#define POOL_SIZE(max_alloc_size) (((max_alloc_size) + (2 * WORD_SIZE)) * ALLOC_PER_POOL + (4 * WORD_SIZE))
/**
 * @note Adjusted size so the size if a multiple of the system page size.
 */
#define POOL_ADJUSTED_SIZE(max_alloc_size) \
    (POOL_SIZE(max_alloc_size) + ((size_t)getpagesize() - POOL_SIZE(max_alloc_size) % (size_t)getpagesize()))

typedef uint8_t  t_byte;
typedef uint32_t t_word;
typedef uint64_t t_dword;

typedef struct s_pool {
    size_t  min_alloc_size;
    size_t  max_alloc_size;
    t_byte *base_ptr;
    /* ptr always point to the prologue block of the pool. */
    t_byte *beginning_ptr;
    size_t  size;
} t_pool;

int   init_pool(t_pool *pool);
void  release_pool(t_pool *pool);
void *find_pool_free_block(t_pool *pool, size_t size);
// void  place_pool_block(t_pool *pool, void *blk);

void show_pool(const t_pool *pool);

#endif