/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_block.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 16:50:12 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/12 14:55:27 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "block.h"
#include "unity.h"

void
setUp() {}

void
tearDown() {}

static void *
put_block(void *mem, uint32_t size, bool allocated, void **payload) {
    t_byte *memb = mem;

    PUT_WORD(memb, PACK_HEADER_FOOTER(DWORD_SIZE + size, allocated ? 0x1 : 0x0));
    memb += WORD_SIZE;
    *payload = memb;
    memb += size;
    PUT_WORD(memb, PACK_HEADER_FOOTER(DWORD_SIZE + size, allocated ? 0x1 : 0x0));
    memb += WORD_SIZE;

    return memb;
}

void
test_PUT_WORD() {
    uint32_t word;
    uint64_t dword;

    PUT_WORD(&word, 0xDEADBEEF);
    PUT_WORD(&dword, 0xDEADBEEF);

    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, word);
    TEST_ASSERT_EQUAL_HEX64(0x00000000DEADBEEF, dword);
}

void
test_GET_WORD() {
    uint32_t word  = 0xDEADBEEF;
    uint64_t dword = 0x00000000DEADBEEF;

    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, GET_WORD(&word));
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, GET_WORD(&dword));
}

void
test_NEXT_PREV_BLOCK_PTR() {
    t_byte  fake_heap[4096]  = {0};
    t_byte *ptr_fake_heap    = fake_heap;
    void   *first_block_ptr  = NULL;
    void   *second_block_ptr = NULL;
    void   *third_block_ptr  = NULL;

    /* Padding */

    PUT_WORD(ptr_fake_heap, 0x0);

    ptr_fake_heap += WORD_SIZE;

    /* First block */

    ptr_fake_heap = put_block(ptr_fake_heap, 8, true, &first_block_ptr);

    /* Second block */

    ptr_fake_heap = put_block(ptr_fake_heap, 16, true, &second_block_ptr);

    /* Third block */

    ptr_fake_heap = put_block(ptr_fake_heap, 64, true, &third_block_ptr);

    TEST_ASSERT_EQUAL_PTR(NEXT_BLOCK_PTR(first_block_ptr), second_block_ptr);
    TEST_ASSERT_EQUAL_PTR(PREV_BLOCK_PTR(second_block_ptr), first_block_ptr);

    TEST_ASSERT_EQUAL_PTR(NEXT_BLOCK_PTR(second_block_ptr), third_block_ptr);
    TEST_ASSERT_EQUAL_PTR(PREV_BLOCK_PTR(third_block_ptr), second_block_ptr);
}

void
test_BLOCK_SIZE() {}

int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_PUT_WORD);
    RUN_TEST(test_GET_WORD);
    RUN_TEST(test_NEXT_PREV_BLOCK_PTR);
    return UNITY_END();
}