/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 13:46:14 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/12 22:10:41 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POOL_H
#define POOL_H

#include <stddef.h>

#include "malloc.h"

#define ALLOC_PER_POOL 100

typedef struct s_pool {
    size_t  min_alloc_size;
    size_t  max_alloc_size;
    t_byte *ptr;
    size_t  size;
} t_pool;

#endif