/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_block.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 16:50:12 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/31 15:30:31 by plouvel          ###   ########.fr       */
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

#define FAKE_HEAP_SIZE (1U << 12)
#define XPND_SIZE (1U << 4)

static t_byte  fake_heap[FAKE_HEAP_SIZE] = {0};
static void   *blk                       = NULL;
static t_list *head                      = NULL;

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
test_can_expand_blk_ORPHEAN() {}

void
test_can_expand_blk_TOO_BIG_TO_FIT_IN_POOL() {
    void *next_blk = NULL;

    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));

    next_blk = NEXT_BLK(blk);

    PUT_WORD(GET_HDR(next_blk), PACK(MIN_BLK_SIZE, FREE));
    PUT_WORD(GET_FTR(next_blk), PACK(MIN_BLK_SIZE, FREE));

    // TEST_ASSERT_FALSE(can_expand_blk(blk, XPND_SIZE, MIN_BLK_SIZE));
}

void
test_expand_blk_NEXT_FREE_BIG_ENOUGH() {
    void  *next_blk = NULL;
    t_list fake_prev_free_blk, fake_next_free_blk;

    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));

    next_blk                = NEXT_BLK(blk);
    head                    = FREE_LIST_ELEM(next_blk);
    fake_prev_free_blk.next = head;
    head->prev              = &fake_prev_free_blk;
    head->next              = &fake_next_free_blk;
    fake_next_free_blk.prev = head;

    PUT_WORD(GET_HDR(next_blk), PACK(MIN_BLK_SIZE + XPND_SIZE, FREE));
    PUT_WORD(GET_FTR(next_blk), PACK(MIN_BLK_SIZE + XPND_SIZE, FREE));

    expand_blk(&head, blk, XPND_SIZE);

    next_blk = NEXT_BLK(blk);

    TEST_ASSERT_EQUAL_PTR(next_blk, head);
    TEST_ASSERT_EQUAL_PTR(&fake_prev_free_blk, head->prev);
    TEST_ASSERT_EQUAL_PTR(&fake_next_free_blk, head->next);
    TEST_ASSERT_EQUAL_PTR(head, fake_prev_free_blk.next);
    TEST_ASSERT_EQUAL_PTR(head, fake_next_free_blk.prev);

    TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE + XPND_SIZE, GET_SIZE(GET_HDR(blk)));

    TEST_ASSERT_EQUAL(FREE, GET_ALLOC(GET_HDR(next_blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, GET_SIZE(GET_HDR(next_blk)));
}

void
test_expand_blk_NEXT_FREE_NOT_BIG_ENOUGH() {
    void *next_blk = NULL;

    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, ALLOCATED));

    next_blk = NEXT_BLK(blk);
    head     = FREE_LIST_ELEM(next_blk);

    PUT_WORD(GET_HDR(next_blk), PACK(MIN_BLK_SIZE, FREE));
    PUT_WORD(GET_FTR(next_blk), PACK(MIN_BLK_SIZE, FREE));

    expand_blk(&head, blk, XPND_SIZE);

    TEST_ASSERT_EQUAL_PTR(NULL, head);

    TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE * 2, GET_SIZE(GET_HDR(blk)));
}

void
test_place_blk_EXACT_SIZE() {
    t_list *head = blk;

    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE, FREE));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE, FREE));

    place_blk(&head, blk, MIN_BLK_SIZE);

    TEST_ASSERT_EQUAL_PTR(NULL, head);
    TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, GET_SIZE(GET_HDR(blk)));
}

void
test_place_blk_NOT_ENOUGH_FREE_SPACE_FOR_NEW_FREE_BLOCK() {
    t_list *head = blk;

    PUT_WORD(GET_HDR(blk), PACK(MIN_BLK_SIZE + MIN_BLK_SIZE / 2, FREE));
    PUT_WORD(GET_FTR(blk), PACK(MIN_BLK_SIZE + MIN_BLK_SIZE / 2, FREE));

    place_blk(&head, blk, MIN_BLK_SIZE);

    TEST_ASSERT_EQUAL_PTR(NULL, head);
    TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(blk)));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE + MIN_BLK_SIZE / 2, GET_SIZE(GET_HDR(blk)));
}

void
test_place_blk_ENOUGH_FREE_SPACE_FOR_NEW_FREE_BLOCK() {
    t_list *head          = blk;
    void   *next_free_blk = NULL;

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
    void   *first_blk = NULL, *second_blk = NULL, *third_blk = NULL;
    void   *rslt = NULL;
    t_list *head = NULL;

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
    void   *first_blk = NULL, *second_blk = NULL, *third_blk = NULL;
    void   *rslt = NULL;
    t_list *head = NULL;

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
    void   *first_blk = NULL, *second_blk = NULL, *third_blk = NULL;
    void   *rslt = NULL;
    t_list *head = NULL;

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
    void   *first_blk = NULL, *second_blk = NULL, *third_blk = NULL;
    void   *rslt = NULL;
    t_list *head = NULL;

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
test_orphean_blk() {
    void *blk0 = NULL;
    void *blk1 = NULL;
    void *blk2 = NULL;
    void *blk3 = NULL;

    blk0 = new_orphean_blk(&head, (1 << 10));

    TEST_ASSERT_TRUE((uintptr_t)blk0 % 16 == 0);
    TEST_ASSERT_EQUAL_PTR(GET_ORPHEAN_ELEM(blk0), head);
    TEST_ASSERT_EQUAL_PTR(NULL, head->prev);
    TEST_ASSERT_EQUAL_PTR(NULL, head->next);
    TEST_ASSERT_TRUE(list_len(head) == 1);

    blk1 = new_orphean_blk(&head, (1 << 12));

    TEST_ASSERT_TRUE((uintptr_t)blk1 % 16 == 0);
    TEST_ASSERT_EQUAL_PTR(GET_ORPHEAN_ELEM(blk1), head);
    TEST_ASSERT_EQUAL_PTR(GET_ORPHEAN_ELEM(blk0), head->next);
    TEST_ASSERT_EQUAL_PTR(NULL, head->prev);
    TEST_ASSERT_TRUE(list_len(head) == 2);

    blk2 = new_orphean_blk(&head, (1 << 20));

    TEST_ASSERT_TRUE((uintptr_t)blk2 % 16 == 0);
    TEST_ASSERT_EQUAL_PTR(GET_ORPHEAN_ELEM(blk2), head);
    TEST_ASSERT_EQUAL_PTR(GET_ORPHEAN_ELEM(blk1), head->next);
    TEST_ASSERT_EQUAL_PTR(NULL, head->prev);
    TEST_ASSERT_TRUE(list_len(head) == 3);

    free_orphean_blk(&head, blk1);

    TEST_ASSERT_EQUAL_PTR(GET_ORPHEAN_ELEM(blk2), head);
    TEST_ASSERT_TRUE(list_len(head) == 2);

    free_orphean_blk(&head, blk2);

    TEST_ASSERT_EQUAL_PTR(GET_ORPHEAN_ELEM(blk0), head);
    TEST_ASSERT_TRUE(list_len(head) == 1);

    free_orphean_blk(&head, blk0);

    TEST_ASSERT_EQUAL_PTR(NULL, head);
    TEST_ASSERT_TRUE(list_len(head) == 0);
}

int
main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_can_expand_blk_ORPHEAN);
    RUN_TEST(test_can_expand_blk_TOO_BIG_TO_FIT_IN_POOL);

    RUN_TEST(test_expand_blk_NEXT_FREE_BIG_ENOUGH);
    RUN_TEST(test_expand_blk_NEXT_FREE_NOT_BIG_ENOUGH);

    RUN_TEST(test_coalesce_blk_NO_ADJACENT_FREE);
    RUN_TEST(test_coalesce_blk_PREV_FREE);
    RUN_TEST(test_coalesce_blk_NEXT_FREE);
    RUN_TEST(test_coalesce_blk_PREV_NEXT_FREE);

    RUN_TEST(test_place_blk_EXACT_SIZE);
    RUN_TEST(test_place_blk_NOT_ENOUGH_FREE_SPACE_FOR_NEW_FREE_BLOCK);
    RUN_TEST(test_place_blk_ENOUGH_FREE_SPACE_FOR_NEW_FREE_BLOCK);

    RUN_TEST(test_orphean_blk);

    return UNITY_END();
}