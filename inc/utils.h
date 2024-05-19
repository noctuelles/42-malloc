/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 16:36:49 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/19 11:11:35 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

typedef uint8_t  t_byte;
typedef uint32_t t_word;
typedef uint64_t t_dword;

size_t align_on_page_size_boundary(size_t initial_size);

#endif
