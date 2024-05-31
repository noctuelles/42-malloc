/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 13:46:14 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/31 15:50:05 by plouvel          ###   ########.fr       */
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
#include "pthread.h"

typedef uint8_t  t_byte;
typedef uint32_t t_word;
typedef uint64_t t_dword;

typedef enum e_pool_type {
    NORMAL_POOL,
    ORPHEAN_POOL,
} t_pool_type;

typedef struct s_pool {
    t_heap heap;
    size_t min_alloc_size;
    size_t max_alloc_size;
    /* In a normal pool, only t*/
    t_list     *head;
    void       *beginning;
    t_pool_type type;
} t_pool;

void   *extend_pool(t_pool *pool, const size_t size);
int     init_pool(t_pool *pool);
t_pool *find_appropriate_pool_for_alloc(t_pool *pools, size_t n, size_t adj_size);
void   *find_fit_in_pool(t_pool *pool, const size_t size);
t_pool *find_blk_in_pools(t_pool *pools, size_t n, void *blk);
void    print_pool(const t_pool *pool, int opts);
void    print_pretty_pool(const t_pool *pool, int opts);

#endif