/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_list.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 10:30:05 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/21 12:20:56 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FREE_LIST_H
#define FREE_LIST_H

#include <stddef.h>

typedef struct s_free_list {
    struct s_free_list *prev;
    struct s_free_list *next;
} t_free_list;

void   move_free_list_ptr(t_free_list **head, t_free_list *free_blk, t_free_list *old_free_blk);
void   push_front_free_list(t_free_list **head, t_free_list *free_blk);
void   delone_free_list(t_free_list **head, t_free_list *free_blk);
size_t free_list_len(t_free_list *head);

#endif