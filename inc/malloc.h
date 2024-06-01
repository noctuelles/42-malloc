/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 15:11:36 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/01 17:29:42 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
#define MALLOC_H

#include "pool.h"

#define SHOW_POOLS(opts)                     \
    do {                                     \
        size_t i = 0;                        \
                                             \
        pthread_mutex_lock(&g_lock);         \
        while (i < N_POOLS) {                \
            print_pool(&g_pools[i], (opts)); \
            i++;                             \
        }                                    \
        pthread_mutex_unlock(&g_lock);       \
    } while (0)

#define N_POOLS 3
#define ORPHEAN_POOL_IDX 2

#define POOL_ONE_MIN_ALLOC_SIZE 1
#define POOL_TWO_MIN_ALLOC_SIZE (POOL_ONE_MAX_ALLOC_SIZE + 1)

/* Adjust the size to a quad word boundary, while keeping the minimum block size of 32 bytes. */
#define ADJ_ALLOC_SIZE(size) (QWORD_SIZE * (((size) + (MIN_BLK_SIZE - 1)) / QWORD_SIZE))

void  *malloc(size_t size);
void  *realloc(void *ptr, size_t size);
void   free(void *ptr);
void  *calloc(size_t nmemb, size_t size);
size_t malloc_usable_size(void *ptr);

#endif