/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 21:51:33 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/21 18:23:51 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEAP_H
#define HEAP_H

/**
 * @file heap.h
 * @brief abstraction of a heap using mmap.
 *
 */

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Allocate
 *
 */
#define RESERVE_SIZE (1UL << 29)

#define PROT_RW (PROT_READ | PROT_WRITE)
#define MAP_PRV_ANON (MAP_ANONYMOUS | MAP_PRIVATE)

typedef struct s_heap {
    uint8_t *max_addr;
    uint8_t *brk;
    uint8_t *base;
} t_heap;

void *sbrk_heap(t_heap *heap, size_t increment);

#endif