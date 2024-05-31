/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 10:30:05 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/31 14:07:26 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct s_list {
    struct s_list *prev;
    struct s_list *next;
} t_list;

void   move_list_elem(t_list **head, t_list *dest_elem, t_list *src_elem);
void   push_front_list(t_list **head, t_list *elem);
void   delone_list(t_list **head, t_list *elem);
size_t list_len(t_list *head);

#endif