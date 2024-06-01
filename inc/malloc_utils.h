/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_utils.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 16:50:00 by plouvel           #+#    #+#             */
/*   Updated: 2024/06/01 17:21:18 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_UTILS_H
#define MALLOC_UTILS_H

#define PRINT_ALLOC (1U << 0)
#define PRINT_FREE (1U << 1)

#define PRINT_ALLOC_HEXDUMP (1U << 2)
#define PRINT_FREE_HEXDUMP (1U << 3)

void show_alloc_mem();
void show_alloc_mem_hex();
void show_free_mem();
void show_free_mem_hex();

#endif