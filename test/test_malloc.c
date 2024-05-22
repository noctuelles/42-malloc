/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_malloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:48:16 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/22 15:54:21 by plouvel          ###   ########.fr       */
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
test_malloc() {}

int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ADJ_ALLOC_SIZE);
    RUN_TEST(test_malloc);
    return UNITY_END();
}