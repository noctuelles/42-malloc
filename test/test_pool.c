/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_pool.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 22:12:20 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/12 22:19:24 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "pool.h"
#include "unity.h"

size_t compute_pool_size(const t_pool *pool);

void
setUp() {}

void
tearDown() {}

void
test_compute_pool_size() {
    t_pool pool = {.min_alloc_size = 1, .max_alloc_size = 1000};

    TEST_ASSERT_EQUAL_UINT64(pool.max_alloc_size * ALLOC_PER_POOL + 2400, compute_pool_size(&pool));
}

int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_compute_pool_size);
    return UNITY_END();
}