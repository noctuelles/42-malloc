/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hexdump.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 16:53:09 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/10 18:51:18 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>

enum e_hexdump_group
{
    WORD = sizeof(uint16_t),
    DWORD = sizeof(uint32_t),
    QWORD = sizeof(uint64_t),
};

#define BYTES_PER_LINE 16
#define GROUP 4
#define PRINT_ASCII

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void print_line_hex(int fd, const void *mem, const size_t bytes_to_print, const size_t n)
{
    size_t i = 0;

    while (i < bytes_to_print)
    {
        dprintf(fd, "%02x", *((uint8_t *)mem + n + i));
        if (i != 0 && (i + 1) % GROUP == 0)
        {
            dprintf(fd, " ");
        }
        i++;
    }
}

void print_line_ascii(int fd, const void *mem, const size_t bytes_to_print, const size_t n)
{
    size_t i = 0;
    char c = 0;

    dprintf(fd, "|");
    while (i < bytes_to_print)
    {
        c = *((char *)mem + n + i);
        if (isprint(c))
        {
            dprintf(fd, "%c", c);
        }
        else
        {
            dprintf(fd, "%c", '.');
        }
        i++;
    }
    dprintf(fd, "|");
}

int get_nbr_spaces(const size_t bytes_to_print)
{
    return (int)((BYTES_PER_LINE * 2 + BYTES_PER_LINE / GROUP) - (bytes_to_print * 2 + bytes_to_print / GROUP));
}

void hexdump(const int fd, const void *mem, const size_t mem_size)
{
    size_t n = 0;
    size_t bytes_to_print = 0;

    assert(BYTES_PER_LINE != 0 && BYTES_PER_LINE % 2 == 0);
    assert(GROUP != 0 && GROUP % 2 == 0 && GROUP <= BYTES_PER_LINE / 2);

    while (n < mem_size)
    {
        bytes_to_print = MIN(mem_size - n, BYTES_PER_LINE);

        dprintf(fd, "%p: ", (uint8_t *)(mem + n));
        print_line_hex(fd, mem, bytes_to_print, n);
#ifdef PRINT_ASCII
        dprintf(fd, "%*s", get_nbr_spaces(bytes_to_print), "");
        print_line_ascii(fd, mem, bytes_to_print, n);
#endif
        n += bytes_to_print;
        dprintf(fd, "\n");
    }
}
