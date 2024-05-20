/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 21:24:07 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/19 22:30:10 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "block.h"

size_t
align_on_page_size_boundary(size_t initial_size) {
    const size_t page_size = (size_t)sysconf(_SC_PAGE_SIZE);
    return (page_size * ((initial_size + (page_size - 1)) / page_size));
}

size_t
align_on_dword_boundary(size_t initial_size) {
    return (DWORD_SIZE * ((initial_size + (DWORD_SIZE - 1)) / DWORD_SIZE));
}