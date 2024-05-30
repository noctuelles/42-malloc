/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 16:36:49 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/30 15:30:51 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

typedef uint8_t  t_byte;
typedef uint32_t t_word;
typedef uint64_t t_dword;

#define WORD_SIZE sizeof(t_word)
#define DWORD_SIZE sizeof(t_dword)
#define QWORD_SIZE (DWORD_SIZE * 2)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

size_t align_on_page_size_boundary(size_t initial_size);
size_t align_on_dword_boundary(size_t initial_size);
void   hexdump(const int fd, const void *mem, const size_t mem_size);

#endif
