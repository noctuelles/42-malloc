/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_malloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:48:16 by plouvel           #+#    #+#             */
/*   Updated: 2024/05/29 16:24:29 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "inc/malloc.h"
#include "malloc_utils.h"
#include "tunable.h"
#include "unity.h"

/* ## UTILS ## */

static size_t
rand_between(size_t min, size_t max) {
    return (size_t)(rand() % (max - min + 1) + min);
}

static void
memrand(void *ptr, size_t size) {
    int devrand = open("/dev/urandom", O_RDONLY);
    if (devrand == -1) {
        return;
    }
    read(devrand, ptr, size);
}

static void (*thread_routine)() = NULL;
void *
thread_routine_wrapper(void *parg) {
    (void)parg;
    thread_routine();
    return (NULL);
}

static pthread_mutex_t lock       = PTHREAD_MUTEX_INITIALIZER;
static size_t          nbr_allocs = 0;
static size_t          nbr_free   = 0;

void
setUp() {
    srand(time(NULL));
    nbr_allocs = 0;
    nbr_free   = 0;
}

void
tearDown() {
    char buffer[256];

    TEST_MESSAGE("Memory usage statistics :");
    snprintf(buffer, 256, "Number of allocations : %zu", nbr_allocs);
    TEST_MESSAGE(buffer);
    snprintf(buffer, 256, "Number of frees : %zu", nbr_free);
    TEST_MESSAGE(buffer);
}

void
test_ADJ_ALLOC_SIZE() {
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, ADJ_ALLOC_SIZE(1));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, ADJ_ALLOC_SIZE(2));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, ADJ_ALLOC_SIZE(7));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, ADJ_ALLOC_SIZE(8));
    TEST_ASSERT_EQUAL(MIN_BLK_SIZE, ADJ_ALLOC_SIZE(16));
}

void
test_malloc_free_RANDOM() {
    /**
     * @param BUFFER_SIZE Larger buffer size results in more memory being allocated than freed.
     */

#define BUFFER_SIZE 1000
#define RUN_TIME_IN_SEC 20
#define MIN_ALLOC_SIZE POOL_ONE_MIN_ALLOC_SIZE
#define MAX_ALLOC_SIZE POOL_TWO_MAX_ALLOC_SIZE

    void           *ptrs[BUFFER_SIZE] = {0};
    char            buffer[1 << 10]   = {0};
    struct timespec start, sample;
    size_t          i          = 0;
    size_t          alloc_size = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &sample);
        if (sample.tv_sec - start.tv_sec >= RUN_TIME_IN_SEC) {
            break;
        }
        i = rand_between(0, BUFFER_SIZE);
        if (ptrs[i] != NULL) {
            free(ptrs[i]);
            ptrs[i] = NULL;

            pthread_mutex_lock(&lock);
            nbr_free++;
            pthread_mutex_unlock(&lock);
        } else {
            alloc_size = rand_between(MIN_ALLOC_SIZE, MAX_ALLOC_SIZE);
            ptrs[i]    = malloc(alloc_size);
            if (ptrs[i] == NULL) {
                TEST_FAIL_MESSAGE("Could not allocate memory");
            }

            memrand(ptrs[i], alloc_size);

            TEST_ASSERT_TRUE((uintptr_t)ptrs[i] % 16 == 0);

            pthread_mutex_lock(&lock);
            nbr_allocs++;
            pthread_mutex_unlock(&lock);
        }
    }
#undef BUFFER_SIZE
#undef RUN_TIME_IN_SEC
#undef MIN_ALLOC_SIZE
#undef MAX_ALLOC_SIZE
}

void
test_malloc_free_RANDOM_MULTITHREADING() {
#define NBR_THREADS 16
    pthread_t threads[NBR_THREADS];
    char      buff[1 << 8] = {0};
    int       ret          = 0;

    thread_routine = test_malloc_free_RANDOM;

    for (size_t i = 0; i < NBR_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, thread_routine_wrapper, NULL) != 0) {
            TEST_FAIL_MESSAGE("Could not create thread.");
        }
    }
    for (size_t i = 0; i < NBR_THREADS; i++) {
        int join_ret = pthread_join(threads[i], NULL);
        if (join_ret != 0) {
            char buffer[256];
            snprintf(buffer, 256, "Could not join thread : %s", strerror(join_ret));
            TEST_FAIL_MESSAGE(buffer);
        }
    }

    TEST_PASS();
#undef NBR_THREADS
}

int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_malloc_free_RANDOM);
    RUN_TEST(test_malloc_free_RANDOM_MULTITHREADING);
    return UNITY_END();
}