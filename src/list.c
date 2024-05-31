/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 18:26:38 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/31 16:44:59 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "list.h"

#include <stddef.h>

void
push_front_list(t_list **head, t_list *elem) {
    if (*head == NULL) {
        elem->next = NULL;
    } else {
        (*head)->prev = elem;
        elem->next    = *head;
    }
    elem->prev = NULL;
    *head      = elem;
}

void
delone_list(t_list **head, t_list *del) {
    if (del == NULL) {
        return;
    }

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
move_list_elem(t_list **head, t_list *dest_blk, t_list *src_blk) {
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
list_len(t_list *head) {
    size_t len = 0;

    while (head != NULL) {
        len++;
        head = head->next;
    }
    return (len);
}