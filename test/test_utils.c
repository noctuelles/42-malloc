/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 11:41:08 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/20 14:46:59 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "unity.h"
#include "utils.h"

static size_t page_size = 0;

void
tearDown() {}

void
setUp() {}

void
test_align_on_page_size_boundary() {
    TEST_ASSERT_EQUAL(page_size, align_on_page_size_boundary(1));
    TEST_ASSERT_EQUAL(page_size, align_on_page_size_boundary(3));
    TEST_ASSERT_EQUAL(page_size, align_on_page_size_boundary(511));
    TEST_ASSERT_EQUAL(page_size, align_on_page_size_boundary(page_size));
    TEST_ASSERT_EQUAL(page_size, align_on_page_size_boundary(page_size - 1));
    TEST_ASSERT_EQUAL(page_size * 2, align_on_page_size_boundary(page_size + 1));
}

void
test_align_on_dword_boundary() {
    TEST_ASSERT_EQUAL(8, align_on_dword_boundary(1));
    TEST_ASSERT_EQUAL(8, align_on_dword_boundary(3));
    TEST_ASSERT_EQUAL(8, align_on_dword_boundary(4));
    TEST_ASSERT_EQUAL(8, align_on_dword_boundary(5));
    TEST_ASSERT_EQUAL(8, align_on_dword_boundary(7));
    TEST_ASSERT_EQUAL(8, align_on_dword_boundary(8));
    TEST_ASSERT_EQUAL(16, align_on_dword_boundary(9));
}

int
main(void) {
    UNITY_BEGIN();
    page_size = (size_t)sysconf(_SC_PAGE_SIZE);
    RUN_TEST(test_align_on_page_size_boundary);
    RUN_TEST(test_align_on_dword_boundary);
    return UNITY_END();
}