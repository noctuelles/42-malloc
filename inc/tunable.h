/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tunable.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 17:09:53 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/29 17:39:20 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TUNABLE_H
#define TUNABLE_H

#include <stddef.h>

#define FT_HEAP_RESERVE_SIZE_STR "FT_HEAP_RESERVE_SIZE"
#define HEAP_RESERVE_SIZE (1UL << 30)

#define FT_POOL_CHUNK_EXTENSION_STR "FT_POOL_CHUNK_EXTENSION"
#define POOL_CHUNK_EXTENSION (1U << 15)

#define FT_POOL_ONE_MAX_ALLOC_SIZE_STR "FT_POOL_ONE_MAX_ALLOC_SIZE"
#define POOL_ONE_MAX_ALLOC_SIZE (1 << 8)

#define FT_POOL_TWO_MAX_ALLOC_SIZE_STR "FT_POOL_TWO_MAX_ALLOC_SIZE"
#define POOL_TWO_MAX_ALLOC_SIZE (1 << 15)

size_t get_tunable(const char *name, size_t default_value);

#endif  // TUNABLE_H