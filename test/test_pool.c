/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_pool.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 16:38:30 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/14 17:49:40 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "block.h"
#include "pool.h"
#include "unity.h"

static t_pool g_fake_pool = {0};

void
setUp() {
    g_fake_pool = (t_pool){
        .min_alloc_size = 1,
        .max_alloc_size = 1024,
        .base_ptr       = NULL,
        .beginning_ptr  = NULL,
        .size           = 0,
    };
}

void
tearDown() {
    release_pool(&g_fake_pool);
}

void
test_init_pool() {
    void *next_blk = NULL;

    if (init_pool(&g_fake_pool) == -1) {
        TEST_FAIL_MESSAGE("cannot initialize pool");
    }

    /* Epilogue Block */

    TEST_ASSERT_EQUAL_UINT32(DWORD_SIZE, GET_SIZE(GET_HEADER(g_fake_pool.beginning_ptr)));
    TEST_ASSERT_EQUAL(ALLOCATED, IS_ALLOCATED(GET_HEADER(g_fake_pool.beginning_ptr)));

    /* Initial Free Block */

    next_blk = NEXT_BLOCK_PTR(g_fake_pool.beginning_ptr);

    TEST_ASSERT_EQUAL(FREE, IS_ALLOCATED(GET_HEADER(next_blk)));

    /* Prologue Block */

    next_blk = NEXT_BLOCK_PTR(next_blk);

    TEST_ASSERT_EQUAL_UINT32(0, GET_SIZE(GET_HEADER(next_blk)));
    TEST_ASSERT_EQUAL(ALLOCATED, IS_ALLOCATED(GET_HEADER(next_blk)));
}

void
test_release_pool() {
    release_pool(&g_fake_pool);

    TEST_ASSERT_EQUAL_PTR(NULL, g_fake_pool.base_ptr);
    TEST_ASSERT_EQUAL_PTR(NULL, g_fake_pool.beginning_ptr);
    TEST_ASSERT_EQUAL_UINT64(0, g_fake_pool.size);
}

void
test_POOL_ADJUSTED_SIZE() {
    size_t page_size = (size_t)getpagesize();

    TEST_ASSERT_EQUAL_UINT64(page_size, POOL_ADJUSTED_SIZE(1));
    TEST_ASSERT_EQUAL_UINT64(page_size, POOL_ADJUSTED_SIZE(2));
}

int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_init_pool);
    RUN_TEST(test_release_pool);
    RUN_TEST(test_POOL_ADJUSTED_SIZE);
    return UNITY_END();
}