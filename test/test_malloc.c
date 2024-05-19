/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_malloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:48:16 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/19 21:07:31 by plouvel          ###   ########.fr       */
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
    void *first_malloc = my_malloc(12);
    void *sncd_malloc  = my_malloc(31);
    void *thrd_malloc  = my_malloc(57);
    void *frth_malloc  = my_malloc(128);
    void *ffth_malloc  = my_malloc(256);
    void *sxth_malloc  = my_malloc(512);

    my_free(first_malloc);
    my_free(frth_malloc);
    my_free(thrd_malloc);

    my_free(ffth_malloc);
    my_free(sxth_malloc);
}

int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ADJ_ALLOC_SIZE);
    RUN_TEST(test_malloc);
    return UNITY_END();
}