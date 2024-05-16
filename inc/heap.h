/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 21:51:33 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/16 22:39:35 by plouvel          ###   ########.fr       */
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

typedef struct s_heap {
    uint8_t *max_addr;
    uint8_t *brk;
    uint8_t *base;
} t_heap;

void *sbrk_heap(t_heap *heap, size_t increment);

#endif