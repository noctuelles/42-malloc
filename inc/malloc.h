/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 15:11:36 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/19 19:17:47 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
#define MALLOC_H

#include "pool.h"

#define POOL_NBR 2
#define ADJ_ALLOC_SIZE(size) (DWORD_SIZE * (((size) + (DWORD_SIZE * 3) + (DWORD_SIZE - 1)) / DWORD_SIZE))

void *my_malloc(size_t size);
void  my_free(void *ptr);

#endif