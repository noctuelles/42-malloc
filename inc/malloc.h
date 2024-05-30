/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 15:11:36 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/30 15:47:02 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
#define MALLOC_H

#include "pool.h"

#define N_POOLS 2

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