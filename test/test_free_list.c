/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_free_list.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 19:56:37 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/22 20:17:54 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "free_list.h"
#include "unity.h"

static t_free_list *head = NULL;

static t_free_list elem0 = {0};
static t_free_list elem1 = {0};
static t_free_list elem2 = {0};
static t_free_list elem3 = {0};

void
setUp(void) {
    elem0.next = &elem1;
    elem0.prev = NULL;

    elem1.next = &elem2;
    elem1.prev = &elem0;

    elem2.next = &elem3;
    elem2.prev = &elem1;

    elem3.next = NULL;
    elem3.prev = &elem2;

    head = &elem0;
}

void
tearDown(void) {}

void
test_delone_free_list_HEAD() {
    delone_free_list(&head, &elem0);

    TEST_ASSERT_EQUAL_PTR(&elem1, head);

    TEST_ASSERT_EQUAL_PTR(NULL, elem1.prev);
    TEST_ASSERT_EQUAL_PTR(&elem2, elem1.next);

    TEST_ASSERT_EQUAL_PTR(&elem1, elem2.prev);
    TEST_ASSERT_EQUAL_PTR(&elem3, elem2.next);

    TEST_ASSERT_EQUAL_PTR(&elem2, elem3.prev);
    TEST_ASSERT_EQUAL_PTR(NULL, elem3.next);
}

void
test_delone_free_list_TAIL() {
    delone_free_list(&head, &elem3);

    TEST_ASSERT_EQUAL_PTR(&elem0, head);

    TEST_ASSERT_EQUAL_PTR(NULL, elem0.prev);
    TEST_ASSERT_EQUAL_PTR(&elem1, elem0.next);

    TEST_ASSERT_EQUAL_PTR(&elem0, elem1.prev);
    TEST_ASSERT_EQUAL_PTR(&elem2, elem1.next);

    TEST_ASSERT_EQUAL_PTR(&elem1, elem2.prev);
    TEST_ASSERT_EQUAL_PTR(NULL, elem2.next);
}

void
test_delone_free_list_MIDDLE() {
    delone_free_list(&head, &elem2);

    TEST_ASSERT_EQUAL_PTR(&elem0, head);

    TEST_ASSERT_EQUAL_PTR(NULL, elem0.prev);
    TEST_ASSERT_EQUAL_PTR(&elem1, elem0.next);

    TEST_ASSERT_EQUAL_PTR(&elem0, elem1.prev);
    TEST_ASSERT_EQUAL_PTR(&elem3, elem1.next);

    TEST_ASSERT_EQUAL_PTR(&elem1, elem3.prev);
    TEST_ASSERT_EQUAL_PTR(NULL, elem3.next);
}

void
test_push_front_free_list_EXISTING_LIST() {
    t_free_list elem4 = {0};

    push_front_free_list(&head, &elem4);

    TEST_ASSERT_EQUAL_PTR(&elem4, head);

    TEST_ASSERT_EQUAL_PTR(NULL, elem4.prev);
    TEST_ASSERT_EQUAL_PTR(&elem0, elem4.next);

    TEST_ASSERT_EQUAL_PTR(&elem4, elem0.prev);
    TEST_ASSERT_EQUAL_PTR(&elem1, elem0.next);

    TEST_ASSERT_EQUAL_PTR(&elem0, elem1.prev);
    TEST_ASSERT_EQUAL_PTR(&elem2, elem1.next);

    TEST_ASSERT_EQUAL_PTR(&elem1, elem2.prev);
    TEST_ASSERT_EQUAL_PTR(&elem3, elem2.next);

    TEST_ASSERT_EQUAL_PTR(&elem2, elem3.prev);
    TEST_ASSERT_EQUAL_PTR(NULL, elem3.next);
}

void
test_push_front_free_list_NEW_LIST() {
    t_free_list *new_head = NULL;
    t_free_list  elem4    = {0};

    push_front_free_list(&new_head, &elem4);

    TEST_ASSERT_EQUAL_PTR(&elem4, new_head);

    TEST_ASSERT_EQUAL_PTR(NULL, elem4.prev);
    TEST_ASSERT_EQUAL_PTR(NULL, elem4.next);
}

void
test_move_free_list_ptr_HEAD() {
    t_free_list elem4 = {0};

    move_free_list_ptr(&head, &elem4, head);

    TEST_ASSERT_EQUAL_PTR(&elem4, head);

    TEST_ASSERT_EQUAL_PTR(NULL, elem4.prev);
    TEST_ASSERT_EQUAL_PTR(&elem1, elem4.next);

    TEST_ASSERT_EQUAL_PTR(&elem4, elem1.prev);
    TEST_ASSERT_EQUAL_PTR(&elem2, elem1.next);

    TEST_ASSERT_EQUAL_PTR(&elem1, elem2.prev);
    TEST_ASSERT_EQUAL_PTR(&elem3, elem2.next);

    TEST_ASSERT_EQUAL_PTR(&elem2, elem3.prev);
    TEST_ASSERT_EQUAL_PTR(NULL, elem3.next);
}

void
test_move_free_list_ptr_MIDDLE() {
    t_free_list elem4 = {0};

    move_free_list_ptr(&head, &elem4, &elem2);

    TEST_ASSERT_EQUAL_PTR(&elem0, head);

    TEST_ASSERT_EQUAL_PTR(NULL, elem0.prev);
    TEST_ASSERT_EQUAL_PTR(&elem1, elem0.next);

    TEST_ASSERT_EQUAL_PTR(&elem0, elem1.prev);
    TEST_ASSERT_EQUAL_PTR(&elem4, elem1.next);

    TEST_ASSERT_EQUAL_PTR(&elem1, elem4.prev);
    TEST_ASSERT_EQUAL_PTR(&elem3, elem4.next);

    TEST_ASSERT_EQUAL_PTR(&elem4, elem3.prev);
    TEST_ASSERT_EQUAL_PTR(NULL, elem3.next);
}

void
test_move_free_list_ptr_TAIL() {
    t_free_list elem4 = {0};

    move_free_list_ptr(&head, &elem3, &elem4);

    TEST_ASSERT_EQUAL_PTR(&elem0, head);

    TEST_ASSERT_EQUAL_PTR(NULL, elem0.prev);
    TEST_ASSERT_EQUAL_PTR(&elem1, elem0.next);

    TEST_ASSERT_EQUAL_PTR(&elem0, elem1.prev);
    TEST_ASSERT_EQUAL_PTR(&elem2, elem1.next);

    TEST_ASSERT_EQUAL_PTR(&elem1, elem2.prev);
    TEST_ASSERT_EQUAL_PTR(&elem4, elem2.next);

    TEST_ASSERT_EQUAL_PTR(&elem2, elem4.prev);
    TEST_ASSERT_EQUAL_PTR(NULL, elem4.next);
}

void
test_free_list_len() {
    TEST_ASSERT_EQUAL_UINT(4, free_list_len(head));
}

int
main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_delone_free_list_HEAD);
    RUN_TEST(test_delone_free_list_TAIL);
    RUN_TEST(test_delone_free_list_MIDDLE);

    RUN_TEST(test_push_front_free_list_EXISTING_LIST);
    RUN_TEST(test_push_front_free_list_NEW_LIST);

    RUN_TEST(test_move_free_list_ptr_HEAD);
    RUN_TEST(test_move_free_list_ptr_MIDDLE);
    RUN_TEST(test_move_free_list_ptr_TAIL);

    RUN_TEST(test_free_list_len);

    return UNITY_END();
}