/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 15:11:36 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/22 16:23:52 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
#define MALLOC_H

#include "pool.h"

#define POOL_NBR 2
#define ADJ_ALLOC_SIZE(size) (QWORD_SIZE * (((size) + (MIN_BLK_SIZE - 1)) / QWORD_SIZE))

void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void  free(void *ptr);

#endif