/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_malloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:48:16 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/23 17:02:38 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/malloc.h"
#include "malloc_utils.h"
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
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, ADJ_ALLOC_SIZE(16));
}

void
test_malloc_FRAGMENTATION() {
#define NBR_ALLOCS 1000
    void *ptrs[NBR_ALLOCS];

    for (size_t i = 0; i < NBR_ALLOCS; i++) {
        void *ptr = malloc(1);
        if (ptr == NULL) {
            TEST_FAIL_MESSAGE("Could not allocate memory");
        }
        ptrs[i] = ptr;
    }
    for (size_t i = 1; i < NBR_ALLOCS; i += 2) {
        free(ptrs[i]);
    }

    show_pools_mem();

    for (size_t i = 0; i < NBR_ALLOCS; i += 2) {
        free(ptrs[i]);
    }

    show_pools_mem();
#undef NBR_ALLOCS
}

int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ADJ_ALLOC_SIZE);
    RUN_TEST(test_malloc_FRAGMENTATION);
    return UNITY_END();
}