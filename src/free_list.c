/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_list.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 18:26:38 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/21 12:20:01 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "free_list.h"

#include <stddef.h>

/**
 * @brief Add a free block to the front of the free list.
 *
 * @param head The head of the free list.
 * @param free_blk The free block to add to the free list.
 */
void
push_front_free_list(t_free_list **head, t_free_list *free_blk) {
    if (*head == NULL) {
        free_blk->next = NULL;
    } else {
        (*head)->prev  = free_blk;
        free_blk->next = *head;
    }
    free_blk->prev = NULL;
    *head          = free_blk;
}

/**
 * @brief Delete a free block from the free list.
 *
 * @param head The head of the free list.
 * @param free_blk The free block to delete from the free list.
 */
void
delone_free_list(t_free_list **head, t_free_list *free_blk) {
    if (*head == free_blk) {
        *head = free_blk->next;

        if (*head) {
            (*head)->prev = NULL;
        }
    } else {
        free_blk->prev->next = free_blk->next;

        if (free_blk->next) {
            free_blk->next->prev = free_blk->prev;
        }
    }
}

/**
 * @brief Move the values of a free block to another free block.
 *
 * @param head The head of the free list.
 * @param free_blk The free block to move the values to.
 * @param old_free_blk The free block to move the values from.
 */
void
move_free_list_ptr(t_free_list **head, t_free_list *free_blk, t_free_list *old_free_blk) {
    free_blk->prev = old_free_blk->prev;
    free_blk->next = old_free_blk->next;

    if (old_free_blk->prev != NULL) {
        old_free_blk->prev->next = free_blk;
    }
    if (old_free_blk->next != NULL) {
        old_free_blk->next->prev = free_blk;
    }
    if (old_free_blk == *head) {
        *head = free_blk;
    }
}

/**
 * @brief Get the length of a free list.
 *
 * @param head The head of the free list.
 * @return size_t The length of the free list.
 */
size_t
free_list_len(t_free_list *head) {
    size_t len = 0;

    while (head != NULL) {
        len++;
        head = head->next;
    }
    return (len);
}