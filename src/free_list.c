/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_list.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 18:26:38 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/27 18:07:31 by plouvel          ###   ########.fr       */
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
delone_free_list(t_free_list **head, t_free_list *del) {
    if (*head == del) {
        *head = del->next;
    }
    if (del->next != NULL) {
        del->next->prev = del->prev;
    }
    if (del->prev != NULL) {
        del->prev->next = del->next;
    }
    del->prev = NULL;
    del->next = NULL;
}

/**
 * @brief Move the values of a free block to another free block.
 *
 * @param head The head of the free list.
 * @param dest_blk The free block to move the values to.
 * @param src_blk The free block to move the values from.
 */
void
move_free_list_ptr(t_free_list **head, t_free_list *dest_blk, t_free_list *src_blk) {
    dest_blk->prev = src_blk->prev;
    dest_blk->next = src_blk->next;

    if (src_blk == *head) {
        *head = dest_blk;
    }
    if (src_blk->prev != NULL) {
        src_blk->prev->next = dest_blk;
    }
    if (src_blk->next != NULL) {
        src_blk->next->prev = dest_blk;
    }
    src_blk->prev = NULL;
    src_blk->next = NULL;
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