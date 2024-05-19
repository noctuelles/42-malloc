/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_heap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 22:00:46 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/19 11:20:01 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>

#include "heap.h"
#include "unity.h"

static t_heap heap;

void
setUp() {}

void
tearDown() {}

void
test_sbrk_heap() {
    char *mptr  = sbrk_heap(&heap, 3);
    char *mptr2 = sbrk_heap(&heap, 3);

    strcpy(mptr, "foo");
    strcpy(mptr2, "bar");

    TEST_ASSERT_EQUAL_PTR(mptr2, mptr + 3);

    char *mptr3 = sbrk_heap(&heap, (size_t)sysconf(_SC_PAGE_SIZE) - 6 + 1);

    strcpy(mptr3, "baz");

    TEST_ASSERT_EQUAL_STRING(mptr, "foo");
    TEST_ASSERT_EQUAL_STRING(mptr2, "bar");
}

int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_sbrk_heap);
    return (UNITY_END());
}