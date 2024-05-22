/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_heap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 22:00:46 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/21 21:15:41 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>

#include "heap.h"
#include "unity.h"

static t_heap heap;
static size_t g_page_size = 0;

void
setUp() {
    bzero(&heap, sizeof(heap));
    g_page_size = (size_t)sysconf(_SC_PAGE_SIZE);
}

void
tearDown() {
    bzero(&heap, sizeof(heap));
}

void
test_sbrk_heap_STR() {
    const char str0[] = "This string is just an example, don't be fool!";
    const char str1[] = "This string is just another example...";
    char      *mptr0  = sbrk_heap(&heap, sizeof(str0));
    char      *mptr1  = sbrk_heap(&heap, sizeof(str1));

    strcpy(mptr0, str0);
    strcpy(mptr1, str1);

    TEST_ASSERT_EQUAL_PTR(mptr1, mptr0 + sizeof(str0));
    TEST_ASSERT_EQUAL_STRING(str0, mptr0);
    TEST_ASSERT_EQUAL_STRING(str1, mptr1);
}

void
test_sbrk_heap_EXPANSION() {
    const char str0[] = "This string is just an example, don't be fool!";
    const char str1[] = "This string is just another example...";
    char      *mptr0  = sbrk_heap(&heap, sizeof(str0));
    char      *mptr1  = sbrk_heap(&heap, sizeof(str1));
    char      *mptr2  = NULL;

    strcpy(mptr0, str0);
    strcpy(mptr1, str1);

    mptr2 = sbrk_heap(&heap, g_page_size + 1);

    TEST_ASSERT_EQUAL_PTR(mptr1, mptr0 + sizeof(str0));
    TEST_ASSERT_EQUAL_STRING(str0, mptr0);
    TEST_ASSERT_EQUAL_STRING(str1, mptr1);
}

int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_sbrk_heap_STR);
    RUN_TEST(test_sbrk_heap_EXPANSION);
    return (UNITY_END());
}