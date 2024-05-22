/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_pool.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 16:38:30 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/22 13:41:24 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "block.h"
#include "pool.h"
#include "unity.h"

// static t_pool g_fake_pool = {0};

void
setUp() {}

void
tearDown() {}

// void
// test_init_pool() {
//     void *next_blk = NULL;

//     if (init_pool(&g_fake_pool) == -1) {
//         TEST_FAIL_MESSAGE("cannot initialize pool");
//     }

//     /* Epilogue Block */

//     TEST_ASSERT_EQUAL_UINT32(DWORD_SIZE, GET_SIZE(GET_HDR(g_fake_pool.beginning_ptr)));
//     TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(g_fake_pool.beginning_ptr)));

//     /* Initial Free Block */

//     next_blk = NEXT_BLK(g_fake_pool.beginning_ptr);

//     TEST_ASSERT_EQUAL(FREE, GET_ALLOC(GET_HDR(next_blk)));

//     /* Prologue Block */

//     next_blk = NEXT_BLK(next_blk);

//     TEST_ASSERT_EQUAL_UINT32(0, GET_SIZE(GET_HDR(next_blk)));
//     TEST_ASSERT_EQUAL(ALLOCATED, GET_ALLOC(GET_HDR(next_blk)));
// }

// void
// test_release_pool() {
//     release_pool(&g_fake_pool);

//     TEST_ASSERT_EQUAL_PTR(NULL, g_fake_pool.base_ptr);
//     TEST_ASSERT_EQUAL_PTR(NULL, g_fake_pool.beginning_ptr);
//     TEST_ASSERT_EQUAL_UINT64(0, g_fake_pool.size);
// }

// void
// test_POOL_ADJUSTED_SIZE() {
//     size_t page_size = (size_t)getpagesize();

//     TEST_ASSERT_EQUAL_UINT64(page_size, POOL_ADJUSTED_SIZE(1));
//     TEST_ASSERT_EQUAL_UINT64(page_size, POOL_ADJUSTED_SIZE(2));
// }

void
test_init_pool() {
    t_pool pool = {0};

    init_pool(&pool);
}

int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_init_pool);
    return UNITY_END();
}