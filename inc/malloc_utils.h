/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_utils.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 16:50:00 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/23 16:53:50 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_UTILS_H
#define MALLOC_UTILS_H

#define PRINT_ALLOC (1U << 0)
#define PRINT_FREE (1U << 1)

void show_alloc_mem();
void show_alloc_mem_hex();

void show_pools_mem();

#endif