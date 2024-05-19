/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 11:41:08 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/19 11:45:11 by plouvel          ###   ########.fr       */
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

int
main(void) {
    UNITY_BEGIN();
    page_size = (size_t)sysconf(_SC_PAGE_SIZE);
    RUN_TEST(test_align_on_page_size_boundary);
    return UNITY_END();
}