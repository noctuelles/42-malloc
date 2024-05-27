/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_block.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 16:50:12 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/27 16:14:57 by plouvel          ###   ########.fr       */
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

// void
// test_expand_blk() {
//     void        *next_blk   = NULL;
//     const size_t xpand_size = DWORD_SIZE;

//     PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
//     PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));

//     next_blk = NEXT_BLK(blk);

//     PUT_WORD(GET_HDR(next_blk), PACK(MIN_BLK_SIZE + xpand_size, FREE));
//     PUT_WORD(GET_FTR(next_blk), PACK(MIN_BLK_SIZE + xpand_size, FREE));
//     head = FREE_LIST_ELEM(next_blk);

//     blk      = expand_blk(&head, blk, xpand_size);
//     next_blk = NEXT_BLK(blk);

//     TEST_ASSERT_EQUAL(MIN_BLK_SIZE + xpand_size, GET_SIZE(GET_HDR(blk)));
//     TEST_ASSERT_EQUAL(MIN_BLK_SIZE, GET_SIZE(GET_HDR(next_blk)));
//     TEST_ASSERT_EQUAL_PTR(FREE_LIST_ELEM(next_blk), head);
//     TEST_ASSERT_TRUE((uintptr_t)next_blk % DWORD_SIZE == 0);
// }

// void
// test_expand_blk_XPND_SIZE_TOO_BIG() {
//     void        *next_blk   = NULL;
//     const size_t xpand_size = MIN_BLK_SIZE * 5;

//     PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
//     PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));

//     next_blk = NEXT_BLK(blk);

//     PUT_WORD(GET_HDR(next_blk), PACK(MIN_BLK_SIZE * 4, FREE));
//     PUT_WORD(GET_FTR(next_blk), PACK(MIN_BLK_SIZE * 4, FREE));
//     head = FREE_LIST_ELEM(next_blk);

//     TEST_ASSERT_EQUAL_PTR(NULL, expand_blk(&head, blk, xpand_size));
// }

// void
// test_expand_blk_TAKE_ADJ_FREE_0() {
//     void *next_blk = NULL;

//     PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
//     PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));

//     next_blk = NEXT_BLK(blk);

//     PUT_WORD(GET_HDR(next_blk), PACK(MIN_BLK_SIZE, FREE));
//     PUT_WORD(GET_FTR(next_blk), PACK(MIN_BLK_SIZE, FREE));
//     head = FREE_LIST_ELEM(next_blk);

//     blk = expand_blk(&head, blk, MIN_BLK_SIZE / 2);

//     TEST_ASSERT_EQUAL_PTR(NULL, head);

//     TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 2, GET_SIZE(GET_HDR(blk)));
// }

// void
// test_expand_blk_TAKE_ADJ_FREE_1() {
//     void *next_blk = NULL;

//     PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
//     PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));

//     next_blk = NEXT_BLK(blk);

//     PUT_WORD(GET_HDR(next_blk), PACK(MIN_BLK_SIZE, FREE));
//     PUT_WORD(GET_FTR(next_blk), PACK(MIN_BLK_SIZE, FREE));
//     head = FREE_LIST_ELEM(next_blk);

//     blk = expand_blk(&head, blk, MIN_BLK_SIZE);

//     TEST_ASSERT_EQUAL_PTR(NULL, head);

//     TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 2, GET_SIZE(GET_HDR(blk)));
// }

// void
// test_shrink_blk_NO_COALESCING() {
//     PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE * 4, ALLOCATED));
//     PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE * 4, ALLOCATED));
//     PUT_WORD(GET_HDR(NEXT_BLK(blk)), PACK(0, 1));

//     TEST_ASSERT_EQUAL_PTR(blk, shrink_blk(&head, blk, MIN_BLK_SIZE));
//     TEST_ASSERT_EQUAL_PTR(NEXT_BLK(blk), head);

//     TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 3, GET_SIZE(GET_HDR(blk)));
//     TEST_ASSERT_EQUAL(MIN_BLK_SIZE, GET_SIZE(GET_HDR(NEXT_BLK(blk))));

//     TEST_ASSERT_EQUAL(1, free_list_len(head));
// }

// void
// test_shrink_blk_WITH_COALESCING() {
//     void *next_blk = NULL;

//     PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE * 4, ALLOCATED));
//     PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE * 4, ALLOCATED));

//     next_blk = NEXT_BLK(blk);

//     PUT_WORD(GET_HDR(next_blk), PACK(MIN_BLK_SIZE, FREE));
//     PUT_WORD(GET_FTR(next_blk), PACK(MIN_BLK_SIZE, FREE));

//     head = FREE_LIST_ELEM(next_blk);

//     next_blk = NEXT_BLK(next_blk);

//     PUT_WORD(GET_HDR(next_blk), PACK(0, ALLOCATED));

//     TEST_ASSERT_EQUAL_PTR(blk, shrink_blk(&head, blk, MIN_BLK_SIZE));
//     TEST_ASSERT_EQUAL_PTR(NEXT_BLK(blk), head);

//     TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 3, GET_SIZE(GET_HDR(blk)));
//     TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 2, GET_SIZE(GET_HDR(NEXT_BLK(blk))));

//     TEST_ASSERT_EQUAL(1, free_list_len(head));
// }

// void
// test_shrink_blk_SHRINK_TOO_SMALL() {
//     PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
//     PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
//     PUT_WORD(GET_HDR(NEXT_BLK(blk)), PACK(0, 1));

//     TEST_ASSERT_EQUAL_PTR(NULL, shrink_blk(&head, blk, 0));
//     TEST_ASSERT_EQUAL_PTR(NULL, shrink_blk(&head, blk, 16));
// }

// void
// test_shrink_blk_SHRINK_TOO_MUCH() {
//     PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
//     PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
//     PUT_WORD(GET_HDR(NEXT_BLK(blk)), PACK(0, 1));

//     TEST_ASSERT_EQUAL_PTR(NULL, shrink_blk(&head, blk, 0));
//     TEST_ASSERT_EQUAL_PTR(NULL, shrink_blk(&head, blk, 8));
// }

// void
// test_print_blk() {
//     PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, FREE));
//     PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, FREE));

//     PUT_WORD((t_byte *)blk + 2 * DWORD_SIZE, 0xDEADBEEF);

//     print_blk(blk);
// }

void
test_place_blk_EXACT_SIZE() {
    t_free_list *head = blk;

    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, FREE));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, FREE));

    place_blk(&head, blk, MIN_BLK_SIZE);

    TEST_ASSERT_EQUAL_PTR(NULL, head);
    TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, GET_SIZE(GET_HDR(blk)));
}

void
test_place_blk_NOT_ENOUGH_FREE_SPACE_FOR_NEW_FREE_BLOCK() {
    t_free_list *head = blk;

    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE + MIN_BLK_SIZE / 2, FREE));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE + MIN_BLK_SIZE / 2, FREE));

    place_blk(&head, blk, MIN_BLK_SIZE);

    TEST_ASSERT_EQUAL_PTR(NULL, head);
    TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE + MIN_BLK_SIZE / 2, GET_SIZE(GET_HDR(blk)));
}

void
test_place_blk_ENOUGH_FREE_SPACE_FOR_NEW_FREE_BLOCK() {
    t_free_list *head          = blk;
    void        *next_free_blk = NULL;

    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE * 3, FREE));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE * 3, FREE));

    place_blk(&head, blk, MIN_BLK_SIZE);

    next_free_blk = NEXT_BLK(blk);

    TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, GET_SIZE(GET_HDR(blk)));

    TEST_ASSERT_EQUAL_PTR(next_free_blk, head);
    TEST_ASSERT_EQUAL(FREE, GET_ALLOC(GET_HDR(next_free_blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 2, GET_SIZE(GET_HDR(next_free_blk)));
}

void
test_coalesce_blk_NO_ADJACENT_FREE() {
    void        *first_blk = NULL, *second_blk = NULL, *third_blk = NULL;
    void        *rslt = NULL;
    t_free_list *head = NULL;

    first_blk = blk;

    PUT_WORD(GET_HDR(first_blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_FTR(first_blk), PACK(MIN_BLK_SIZE, ALLOCATED));

    second_blk = NEXT_BLK(first_blk);

    PUT_WORD(GET_HDR(second_blk), PACK(MIN_BLK_SIZE, FREE));
    PUT_WORD(GET_FTR(second_blk), PACK(MIN_BLK_SIZE, FREE));

    third_blk = NEXT_BLK(second_blk);

    PUT_WORD(GET_HDR(third_blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_FTR(third_blk), PACK(MIN_BLK_SIZE, ALLOCATED));

    rslt = coalesce_blk(&head, second_blk);

    TEST_ASSERT_EQUAL_PTR(second_blk, rslt);
    TEST_ASSERT_EQUAL_PTR(second_blk, head);

    TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(first_blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, GET_SIZE(GET_HDR(first_blk)));

    TEST_ASSERT_EQUAL(FREE, GET_ALLOC(GET_HDR(second_blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, GET_SIZE(GET_HDR(second_blk)));

    TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(third_blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, GET_SIZE(GET_HDR(third_blk)));
}

void
test_coalesce_blk_PREV_FREE() {
    void        *first_blk = NULL, *second_blk = NULL, *third_blk = NULL;
    void        *rslt = NULL;
    t_free_list *head = NULL;

    first_blk = blk;
    head      = first_blk;

    PUT_WORD(GET_HDR(first_blk), PACK(MIN_BLK_SIZE * 2, FREE));
    PUT_WORD(GET_FTR(first_blk), PACK(MIN_BLK_SIZE * 2, FREE));

    second_blk = NEXT_BLK(first_blk);

    PUT_WORD(GET_HDR(second_blk), PACK(MIN_BLK_SIZE, FREE));
    PUT_WORD(GET_FTR(second_blk), PACK(MIN_BLK_SIZE, FREE));

    third_blk = NEXT_BLK(second_blk);

    PUT_WORD(GET_HDR(third_blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_FTR(third_blk), PACK(MIN_BLK_SIZE, ALLOCATED));

    rslt = coalesce_blk(&head, second_blk);

    TEST_ASSERT_EQUAL_PTR(first_blk, rslt);
    TEST_ASSERT_EQUAL_PTR(first_blk, head);

    TEST_ASSERT_EQUAL(FREE, GET_ALLOC(GET_HDR(first_blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 2 + MIN_BLK_SIZE, GET_SIZE(GET_HDR(first_blk)));

    TEST_ASSERT_EQUAL(third_blk, NEXT_BLK(first_blk));
    TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(third_blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, GET_SIZE(GET_HDR(third_blk)));
}

void
test_coalesce_blk_NEXT_FREE() {
    void        *first_blk = NULL, *second_blk = NULL, *third_blk = NULL;
    void        *rslt = NULL;
    t_free_list *head = NULL;

    first_blk = blk;

    PUT_WORD(GET_HDR(first_blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_FTR(first_blk), PACK(MIN_BLK_SIZE, ALLOCATED));

    second_blk = NEXT_BLK(first_blk);

    PUT_WORD(GET_HDR(second_blk), PACK(MIN_BLK_SIZE, FREE));
    PUT_WORD(GET_FTR(second_blk), PACK(MIN_BLK_SIZE, FREE));

    third_blk = NEXT_BLK(second_blk);
    head      = third_blk;

    PUT_WORD(GET_HDR(third_blk), PACK(MIN_BLK_SIZE * 3, FREE));
    PUT_WORD(GET_FTR(third_blk), PACK(MIN_BLK_SIZE * 3, FREE));

    rslt = coalesce_blk(&head, second_blk);

    TEST_ASSERT_EQUAL_PTR(second_blk, rslt);
    TEST_ASSERT_EQUAL_PTR(second_blk, head);

    TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(first_blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, GET_SIZE(GET_HDR(first_blk)));

    TEST_ASSERT_EQUAL(FREE, GET_ALLOC(GET_HDR(second_blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 3 + MIN_BLK_SIZE, GET_SIZE(GET_HDR(second_blk)));
}

void
test_coalesce_blk_PREV_NEXT_FREE() {
    void        *first_blk = NULL, *second_blk = NULL, *third_blk = NULL;
    void        *rslt = NULL;
    t_free_list *head = NULL;

    first_blk = blk;
    head      = first_blk;

    PUT_WORD(GET_HDR(first_blk), PACK(MIN_BLK_SIZE * 5, FREE));
    PUT_WORD(GET_FTR(first_blk), PACK(MIN_BLK_SIZE * 5, FREE));

    second_blk = NEXT_BLK(first_blk);

    PUT_WORD(GET_HDR(second_blk), PACK(MIN_BLK_SIZE, FREE));
    PUT_WORD(GET_FTR(second_blk), PACK(MIN_BLK_SIZE, FREE));

    third_blk = NEXT_BLK(second_blk);

    head->prev                      = NULL;
    head->next                      = third_blk;
    FREE_LIST_ELEM(third_blk)->prev = first_blk;
    FREE_LIST_ELEM(third_blk)->next = NULL;

    PUT_WORD(GET_HDR(third_blk), PACK(MIN_BLK_SIZE * 3, FREE));
    PUT_WORD(GET_FTR(third_blk), PACK(MIN_BLK_SIZE * 3, FREE));

    rslt = coalesce_blk(&head, second_blk);

    TEST_ASSERT_EQUAL_PTR(first_blk, rslt);
    TEST_ASSERT_EQUAL_PTR(first_blk, head);
    TEST_ASSERT_EQUAL_PTR(NULL, head->prev);
    TEST_ASSERT_EQUAL_PTR(NULL, head->next);

    TEST_ASSERT_EQUAL(FREE, GET_ALLOC(GET_HDR(first_blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 5 + MIN_BLK_SIZE + MIN_BLK_SIZE * 3, GET_SIZE(GET_HDR(first_blk)));
}

void
test_fill_orphean_blk() {
    blk = fill_orphean_blk(fake_heap, UINT64_MAX);

    TEST_ASSERT_EQUAL(0, GET_SIZE(GET_HDR(blk)));

    TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(blk)));
    TEST_ASSERT_EQUAL(ORPHEAN, GET_ORPHEAN(GET_HDR(blk)));

    TEST_ASSERT_EQUAL_UINT64(UINT64_MAX, GET_ORPHEAN_SIZE(GET_HDR(blk)));
    TEST_ASSERT_TRUE((uintptr_t)blk % QWORD_SIZE == 0);
}

int
main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_coalesce_blk_NO_ADJACENT_FREE);
    RUN_TEST(test_coalesce_blk_PREV_FREE);
    RUN_TEST(test_coalesce_blk_NEXT_FREE);
    RUN_TEST(test_coalesce_blk_PREV_NEXT_FREE);

    RUN_TEST(test_place_blk_EXACT_SIZE);
    RUN_TEST(test_place_blk_NOT_ENOUGH_FREE_SPACE_FOR_NEW_FREE_BLOCK);
    RUN_TEST(test_place_blk_ENOUGH_FREE_SPACE_FOR_NEW_FREE_BLOCK);

    RUN_TEST(test_fill_orphean_blk);

    return UNITY_END();
}