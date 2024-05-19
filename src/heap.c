/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 20:38:56 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/18 21:25:16 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"

#include <assert.h>
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
init_heap(t_heap *heap, size_t initial_size) {
    initial_size = align_on_page_size_boundary(initial_size);

    heap->base = mmap(NULL, initial_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (heap->base == MAP_FAILED) {
        return ((void *)-1);
    }
    heap->brk      = heap->base;
    heap->max_addr = heap->base + initial_size;
    return (heap->brk);
}

/**
 * @brief Increase the heap size by increment.
 *
 * @param heap
 * @param increment
 * @return void*
 */
void *
sbrk_heap(t_heap *heap, size_t increment) {
    uint8_t *old_brk             = heap->brk;
    size_t   heap_extension_size = 0;

    if (heap->base == NULL) {
        return (init_heap(heap, increment));
    }
    if (heap->brk + increment > heap->max_addr) {
        heap_extension_size = align_on_page_size_boundary(increment);
        if (mmap(heap->max_addr, heap_extension_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                 -1, 0) == MAP_FAILED) {
            return ((void *)-1);
        }
        heap->max_addr += heap_extension_size;
    }
    heap->brk += increment;
    return (old_brk);
}