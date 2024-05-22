/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 13:46:14 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/22 15:09:38 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POOL_H
#define POOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "block.h"
#include "heap.h"
#include "inc/malloc.h"

#define N_POOLS 2
#define MIN_ALLOC_PER_POOL 100

#define POOL_ONE_MIN_ALLOC_SIZE 1
#define POOL_ONE_MAX_ALLOC_SIZE 512

#define POOL_TWO_MIN_ALLOC_SIZE (POOL_ONE_MAX_ALLOC_SIZE + 1)
#define POOL_TWO_MAX_ALLOC_SIZE 4096

#define ADJ_POOL_SIZE(max_alloc_size, page_size) \
    ((page_size) * (ADJ_ALLOC_SIZE(max_alloc_size) * MIN_ALLOC_PER_POOL + (page_size)))

#define POOL_CHUNK_EXTENSION (1U << 12)

typedef uint8_t  t_byte;
typedef uint32_t t_word;
typedef uint64_t t_dword;

typedef struct s_pool {
    size_t min_alloc_size;
    size_t max_alloc_size;

    t_heap       heap;
    t_free_list *head;
    void        *beginning;
} t_pool;

void   *extend_pool(t_pool *pool, const size_t size);
int     init_pool(t_pool *pool);
void   *find_fit_in_pool(t_pool *pool, const size_t size);
t_pool *find_blk_in_pools(t_pool *pools, size_t n, void *blk);
void   *find_fit_in_pools(t_pool *pools, size_t n, const size_t adj_size, t_pool **blk_pool);
void    print_pool(const t_pool *pool, bool show_free_blks);
void    print_pretty_pool(const t_pool *pool, bool show_free_blks);

#endif