/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_main.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:25:18 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/10 15:12:15 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "unity.h"
#include "malloc.h"

void setUp()
{
}

void tearDown()
{
}

void test_dummy()
{
    TEST_ASSERT_EQUAL_CHAR('c', 'c');
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_dummy);
    return UNITY_END();
}