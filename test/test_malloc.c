/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_malloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:48:16 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/22 14:25:27 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/malloc.h"
#include "unity.h"

void
setUp() {}

void
tearDown() {}

void
test_ADJ_ALLOC_SIZE() {
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, ADJ_ALLOC_SIZE(1));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, ADJ_ALLOC_SIZE(2));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, ADJ_ALLOC_SIZE(7));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, ADJ_ALLOC_SIZE(8));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE + (1 * DWORD_SIZE), ADJ_ALLOC_SIZE(9));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE + (1 * DWORD_SIZE), ADJ_ALLOC_SIZE(16));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE + (2 * DWORD_SIZE), ADJ_ALLOC_SIZE(17));
}

void
test_malloc() {
    void *first_malloc  = my_malloc(12);
    void *second_malloc = my_malloc(34);
    void *third_malloc  = my_malloc(56);
    void *fourth_malloc = my_malloc(78);

    my_free(first_malloc);
    my_free(third_malloc);
    my_free(second_malloc);
    my_malloc(12);
}

int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ADJ_ALLOC_SIZE);
    RUN_TEST(test_malloc);
    return UNITY_END();
}