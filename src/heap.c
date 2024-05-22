/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 20:38:56 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/21 20:55:29 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

#include "utils.h"

/**
 * @brief Initialize a heap structure.
 *
 * @param heap
 * @param initial_size hint for the initial size of the heap.
 * @return void* the start of the heap.
 */
static void *
init_heap(t_heap *heap) {
    heap->base = mmap(NULL, RESERVE_SIZE, PROT_NONE, MAP_PRV_ANON, -1, 0);
    if (heap->base == MAP_FAILED) {
        return ((void *)-1);
    }
    heap->brk      = heap->base;
    heap->max_addr = heap->base;
    return (heap->brk);
}

/**
 * @brief Extend the heap by extend_size.
 *
 * @param heap
 * @param extend_size
 * @return void*
 */
static void *
extend_heap(t_heap *heap, size_t extend_size) {
    int      flags                 = MAP_PRV_ANON;
    uint8_t *max_reserved_va_space = heap->base + RESERVE_SIZE;

    extend_size = align_on_page_size_boundary(extend_size);
    if (heap->max_addr + extend_size > max_reserved_va_space) {
        flags |= MAP_FIXED_NOREPLACE;
    } else {
        flags |= MAP_FIXED;
    }
    if (mmap(heap->max_addr, extend_size, PROT_RW, flags, -1, 0) == MAP_FAILED) {
        errno = ENOMEM;
        return ((void *)-1);
    }
    heap->max_addr += extend_size;
    return (heap->max_addr);
}

/**
 * @brief Increase the heap size by increment. This is an abstraction over the system sbrk.
 *
 * @param heap
 * @param increment
 * @return void*
 */
void *
sbrk_heap(t_heap *heap, size_t increment) {
    uint8_t *old_brk = NULL;

    if (heap->base == NULL) {
        if (init_heap(heap) == (void *)-1) {
            return ((void *)-1);
        }
    }
    old_brk = heap->brk;
    if (heap->brk + increment > heap->max_addr) {
        if (extend_heap(heap, increment) == (void *)-1) {
            return ((void *)-1);
        }
    }
    heap->brk += increment;
    return (old_brk);
}