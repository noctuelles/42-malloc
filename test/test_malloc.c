/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_malloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:48:16 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/15 16:24:40 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include "unity.h"

void
setUp() {}

void
tearDown() {}

void
test_adj_alloc_size() {
    TEST_ASSERT_EQUAL(16, ADJ_ALLOC_SIZE(1));
    TEST_ASSERT_EQUAL(16, ADJ_ALLOC_SIZE(8));

    TEST_ASSERT_EQUAL(24, ADJ_ALLOC_SIZE(9));
    TEST_ASSERT_EQUAL(24, ADJ_ALLOC_SIZE(15));
    TEST_ASSERT_EQUAL(24, ADJ_ALLOC_SIZE(16));

    TEST_ASSERT_EQUAL(32, ADJ_ALLOC_SIZE(17));
    TEST_ASSERT_EQUAL(32, ADJ_ALLOC_SIZE(24));
}

int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_adj_alloc_size);
    return UNITY_END();
}