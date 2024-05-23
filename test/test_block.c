/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_block.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 16:50:12 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/23 16:44:11 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "block.h"
#include "unity.h"

#define FAKE_HEAP_SIZE 1 << 12

static t_byte       fake_heap[FAKE_HEAP_SIZE] = {0};
static void        *blk                       = NULL;
static t_free_list *head                      = NULL;

void
setUp() {
    PUT_WORD(fake_heap, 0x42424242);
    blk = fake_heap + (2 * WORD_SIZE);
}

void
tearDown() {
    bzero(fake_heap, FAKE_HEAP_SIZE);
    head = NULL;
}

void
test_expand_blk() {
    void        *next_blk   = NULL;
    const size_t xpand_size = DWORD_SIZE;

    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));

    next_blk = NEXT_BLK(blk);

    PUT_WORD(GET_HDR(next_blk), PACK(MIN_BLK_SIZE + xpand_size, FREE));
    PUT_WORD(GET_FTR(next_blk), PACK(MIN_BLK_SIZE + xpand_size, FREE));
    head = FREE_LIST_ELEM(next_blk);

    blk      = expand_blk(&head, blk, xpand_size);
    next_blk = NEXT_BLK(blk);

    TEST_ASSERT_EQUAL(MIN_BLK_SIZE + xpand_size, GET_SIZE(GET_HDR(blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, GET_SIZE(GET_HDR(next_blk)));
    TEST_ASSERT_EQUAL_PTR(FREE_LIST_ELEM(next_blk), head);
    TEST_ASSERT_TRUE((uintptr_t)next_blk % DWORD_SIZE == 0);
}

void
test_expand_blk_XPND_SIZE_TOO_BIG() {
    void        *next_blk   = NULL;
    const size_t xpand_size = MIN_BLK_SIZE * 5;

    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));

    next_blk = NEXT_BLK(blk);

    PUT_WORD(GET_HDR(next_blk), PACK(MIN_BLK_SIZE * 4, FREE));
    PUT_WORD(GET_FTR(next_blk), PACK(MIN_BLK_SIZE * 4, FREE));
    head = FREE_LIST_ELEM(next_blk);

    TEST_ASSERT_EQUAL_PTR(NULL, expand_blk(&head, blk, xpand_size));
}

void
test_expand_blk_TAKE_ADJ_FREE_0() {
    void *next_blk = NULL;

    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));

    next_blk = NEXT_BLK(blk);

    PUT_WORD(GET_HDR(next_blk), PACK(MIN_BLK_SIZE, FREE));
    PUT_WORD(GET_FTR(next_blk), PACK(MIN_BLK_SIZE, FREE));
    head = FREE_LIST_ELEM(next_blk);

    blk = expand_blk(&head, blk, MIN_BLK_SIZE / 2);

    TEST_ASSERT_EQUAL_PTR(NULL, head);

    TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 2, GET_SIZE(GET_HDR(blk)));
}

void
test_expand_blk_TAKE_ADJ_FREE_1() {
    void *next_blk = NULL;

    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));

    next_blk = NEXT_BLK(blk);

    PUT_WORD(GET_HDR(next_blk), PACK(MIN_BLK_SIZE, FREE));
    PUT_WORD(GET_FTR(next_blk), PACK(MIN_BLK_SIZE, FREE));
    head = FREE_LIST_ELEM(next_blk);

    blk = expand_blk(&head, blk, MIN_BLK_SIZE);

    TEST_ASSERT_EQUAL_PTR(NULL, head);

    TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 2, GET_SIZE(GET_HDR(blk)));
}

void
test_shrink_blk_NO_COALESCING() {
    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE * 4, ALLOCATED));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE * 4, ALLOCATED));
    PUT_WORD(GET_HDR(NEXT_BLK(blk)), PACK(0, 1));

    TEST_ASSERT_EQUAL_PTR(blk, shrink_blk(&head, blk, MIN_BLK_SIZE));
    TEST_ASSERT_EQUAL_PTR(NEXT_BLK(blk), head);

    TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 3, GET_SIZE(GET_HDR(blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, GET_SIZE(GET_HDR(NEXT_BLK(blk))));

    TEST_ASSERT_EQUAL(1, free_list_len(head));
}

void
test_shrink_blk_WITH_COALESCING() {
    void *next_blk = NULL;

    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE * 4, ALLOCATED));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE * 4, ALLOCATED));

    next_blk = NEXT_BLK(blk);

    PUT_WORD(GET_HDR(next_blk), PACK(MIN_BLK_SIZE, FREE));
    PUT_WORD(GET_FTR(next_blk), PACK(MIN_BLK_SIZE, FREE));

    head = FREE_LIST_ELEM(next_blk);

    next_blk = NEXT_BLK(next_blk);

    PUT_WORD(GET_HDR(next_blk), PACK(0, ALLOCATED));

    TEST_ASSERT_EQUAL_PTR(blk, shrink_blk(&head, blk, MIN_BLK_SIZE));
    TEST_ASSERT_EQUAL_PTR(NEXT_BLK(blk), head);

    TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 3, GET_SIZE(GET_HDR(blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 2, GET_SIZE(GET_HDR(NEXT_BLK(blk))));

    TEST_ASSERT_EQUAL(1, free_list_len(head));
}

void
test_shrink_blk_SHRINK_TOO_SMALL() {
    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_HDR(NEXT_BLK(blk)), PACK(0, 1));

    TEST_ASSERT_EQUAL_PTR(NULL, shrink_blk(&head, blk, 0));
    TEST_ASSERT_EQUAL_PTR(NULL, shrink_blk(&head, blk, 16));
}

void
test_shrink_blk_SHRINK_TOO_MUCH() {
    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_HDR(NEXT_BLK(blk)), PACK(0, 1));

    TEST_ASSERT_EQUAL_PTR(NULL, shrink_blk(&head, blk, 0));
    TEST_ASSERT_EQUAL_PTR(NULL, shrink_blk(&head, blk, 8));
}

void
test_print_blk() {
    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, FREE));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, FREE));

    PUT_WORD((t_byte *)blk + 2 * DWORD_SIZE, 0xDEADBEEF);

    print_blk(blk);
}

// void
// sig_abrt(int) {
//     TEST_ABORT();
// }

// void
// test_PUT_WORD() {
//     uint32_t word;
//     uint64_t dword;

//     PUT_WORD(&word, 0xDEADBEEF);
//     PUT_WORD(&dword, 0xDEADBEEF);

//     TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, word);
//     TEST_ASSERT_EQUAL_HEX64(0x00000000DEADBEEF, dword);
// }

// void
// test_GET_WORD() {
//     uint32_t word  = 0xDEADBEEF;
//     uint64_t dword = 0x00000000DEADBEEF;

//     TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, GET_WORD(&word));
//     TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, GET_WORD(&dword));
// }

void
test_place_block_INSIDE_LARGE_FREE() {}

// void
// test_place_block_EXACT_SIZE() {
//     const size_t blk_size = 16;

//     ptr_fake_heap += WORD_SIZE;

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(blk_size, FREE));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(blk_size, FREE));

//     place_block(ptr_fake_heap, blk_size);

//     TEST_ASSERT_EQUAL(blk_size, GET_SIZE(GET_HDR(ptr_fake_heap)));
//     TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(ptr_fake_heap)));

//     ptr_fake_heap = NEXT_BLK(ptr_fake_heap);

//     TEST_ASSERT_EQUAL_UINT32(0U, GET_WORD(GET_HDR(ptr_fake_heap)));
// }

// void
// test_place_block_WRONG_SIZE() {
//     const size_t blk_size = FAKE_HEAP_SIZE + 1;

//     signal(SIGABRT, sig_abrt);

//     ptr_fake_heap += WORD_SIZE;

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(FAKE_HEAP_SIZE, FREE));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(FAKE_HEAP_SIZE, FREE));

//     place_block(ptr_fake_heap, blk_size);

//     TEST_FAIL();
// }

// void
// test_place_block_NOT_FREE() {
//     const size_t blk_size = 16;

//     (void)signal(SIGABRT, sig_abrt);

//     ptr_fake_heap += WORD_SIZE;

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(FAKE_HEAP_SIZE, ALLOCATED));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(FAKE_HEAP_SIZE, ALLOCATED));

//     place_block(ptr_fake_heap, blk_size);

//     TEST_FAIL();
// }

// void
// test_coalesce_block_NO_ADJACENT_FREE() {
//     void *first_blk = NULL, *second_blk = NULL, *third_blk = NULL;
//     void *rslt = NULL;

//     ptr_fake_heap = first_blk = ptr_fake_heap + WORD_SIZE;

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(16, ALLOCATED));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(16, ALLOCATED));

//     ptr_fake_heap = second_blk = NEXT_BLK(ptr_fake_heap);

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(16, FREE));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(16, FREE));

//     ptr_fake_heap = third_blk = NEXT_BLK(ptr_fake_heap);

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(16, ALLOCATED));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(16, ALLOCATED));

//     rslt = coalesce_blk(second_blk);

//     TEST_ASSERT_EQUAL_PTR(second_blk, rslt);

//     TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(first_blk)));
//     TEST_ASSERT_EQUAL(16, GET_SIZE(GET_HDR(first_blk)));

//     TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(third_blk)));
//     TEST_ASSERT_EQUAL(16, GET_SIZE(GET_HDR(third_blk)));
// }

// void
// test_coalesce_block_PREV_ADJACENT_FREE() {
//     void *first_blk = NULL, *second_blk = NULL, *third_blk = NULL;
//     void *rslt = NULL;

//     ptr_fake_heap = first_blk = ptr_fake_heap + WORD_SIZE;

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(16, FREE));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(16, FREE));

//     ptr_fake_heap = second_blk = NEXT_BLK(ptr_fake_heap);

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(32, FREE));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(32, FREE));

//     ptr_fake_heap = third_blk = NEXT_BLK(ptr_fake_heap);

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(16, ALLOCATED));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(16, ALLOCATED));

//     rslt = coalesce_blk(second_blk);

//     TEST_ASSERT_EQUAL_PTR(first_blk, rslt);

//     TEST_ASSERT_EQUAL(FREE, GET_ALLOC(GET_HDR(first_blk)));
//     TEST_ASSERT_EQUAL(32 + 16, GET_SIZE(GET_HDR(first_blk)));

//     TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(third_blk)));
//     TEST_ASSERT_EQUAL(16, GET_SIZE(GET_HDR(third_blk)));
// }

// void
// test_coalesce_block_NEXT_ADJACENT_FREE() {
//     void *first_blk = NULL, *second_blk = NULL, *third_blk = NULL;
//     void *rslt = NULL;

//     ptr_fake_heap = first_blk = ptr_fake_heap + WORD_SIZE;

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(16, ALLOCATED));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(16, ALLOCATED));

//     ptr_fake_heap = second_blk = NEXT_BLK(ptr_fake_heap);

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(32, FREE));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(32, FREE));

//     ptr_fake_heap = third_blk = NEXT_BLK(ptr_fake_heap);

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(64, FREE));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(64, FREE));

//     rslt = coalesce_blk(second_blk);

//     TEST_ASSERT_EQUAL_PTR(second_blk, rslt);

//     TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(first_blk)));
//     TEST_ASSERT_EQUAL(16, GET_SIZE(GET_HDR(first_blk)));

//     TEST_ASSERT_EQUAL(FREE, GET_ALLOC(GET_HDR(second_blk)));
//     TEST_ASSERT_EQUAL(32 + 64, GET_SIZE(GET_HDR(second_blk)));
// }

// void
// test_coalesce_block_PREV_NEXT_ADJACENT_FREE() {
//     void *first_blk = NULL, *second_blk = NULL, *third_blk = NULL;
//     void *rslt = NULL;

//     ptr_fake_heap = first_blk = ptr_fake_heap + WORD_SIZE;

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(16, FREE));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(16, FREE));

//     ptr_fake_heap = second_blk = NEXT_BLK(ptr_fake_heap);

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(32, FREE));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(32, FREE));

//     ptr_fake_heap = third_blk = NEXT_BLK(ptr_fake_heap);

//     PUT_WORD(GET_HDR(ptr_fake_heap), PACK(64, FREE));
//     PUT_WORD(GET_FTR(ptr_fake_heap), PACK(64, FREE));

//     rslt = coalesce_blk(second_blk);

//     TEST_ASSERT_EQUAL_PTR(first_blk, rslt);

//     TEST_ASSERT_EQUAL(FREE, GET_ALLOC(GET_HDR(first_blk)));
//     TEST_ASSERT_EQUAL(16 + 32 + 64, GET_SIZE(GET_HDR(first_blk)));
// }

// void
// test_new_anonymous_block() {
//     void  *blk       = NULL;
//     size_t page_size = (size_t)getpagesize();

//     blk = new_anonymous_block(16);

//     if (blk == NULL) {
//         TEST_FAIL_MESSAGE("new_anonymous_block() failed");
//     }

//     TEST_ASSERT_NOT_NULL(blk);

//     TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(blk)));
//     TEST_ASSERT_EQUAL(ANONYMOUS, GET_ANONYMOUS(GET_HDR(blk)));

//     TEST_ASSERT_EQUAL(0, GET_SIZE(GET_HDR(blk)));
//     TEST_ASSERT_EQUAL_UINT64(page_size, GET_ANON_SIZE(GET_HDR(blk)));

//     /* Check if the address is aligned to a double word boundary */

//     TEST_ASSERT_EQUAL(0, ((uintptr_t)blk & 0x7));
// }

void
test_fill_anonymous_blk() {
    blk = fill_anonymous_blk(fake_heap, UINT64_MAX);

    TEST_ASSERT_EQUAL(0, GET_SIZE(GET_HDR(blk)));

    TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(blk)));
    TEST_ASSERT_EQUAL(ANONYMOUS, GET_ANONYMOUS(GET_HDR(blk)));

    TEST_ASSERT_EQUAL_UINT64(UINT64_MAX, GET_ANON_SIZE(GET_HDR(blk)));
    TEST_ASSERT_TRUE((uintptr_t)blk % QWORD_SIZE == 0);
}

int
main(void) {
    UNITY_BEGIN();

    // RUN_TEST(test_expand_blk);
    // RUN_TEST(test_expand_blk_XPND_SIZE_TOO_BIG);
    // RUN_TEST(test_expand_blk_TAKE_ADJ_FREE_0);
    // RUN_TEST(test_expand_blk_TAKE_ADJ_FREE_1);

    // RUN_TEST(test_shrink_blk_NO_COALESCING);
    // RUN_TEST(test_shrink_blk_SHRINK_TOO_SMALL);
    // RUN_TEST(test_shrink_blk_SHRINK_TOO_MUCH);
    // RUN_TEST(test_shrink_blk_WITH_COALESCING);

    RUN_TEST(test_fill_anonymous_blk);

    return UNITY_END();
}