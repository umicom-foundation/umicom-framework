/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/suite.c
 *
 * PURPOSE:
 *   Implement bounded test-suite ownership and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/suite.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiTestSuite {
    char suite_id[UMI_TEST_ID_CAPACITY];
    char name[UMI_TEST_NAME_CAPACITY];
    UmiTestCase *cases;
    size_t count;
};

/*
 * Initialise test suite from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_test_suite_create(const char *suite_id,
                                const char *name,
                                UmiTestSuite **out_suite)
{
    UmiTestSuite *suite;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (suite_id == NULL || name == NULL || out_suite == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_suite = NULL;
    suite = (UmiTestSuite *)calloc(1U, sizeof(*suite));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (suite == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    suite->cases = (UmiTestCase *)calloc(UMI_TEST_SUITE_MAX_CASES,
                                         sizeof(*suite->cases));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (suite->cases == NULL) {
        free(suite);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)snprintf(suite->suite_id,
                   sizeof(suite->suite_id),
                   "%s",
                   suite_id);
    (void)snprintf(suite->name, sizeof(suite->name), "%s", name);
    *out_suite = suite;
    return UMI_STATUS_OK;
}

/* Release or reset state held by test suite so the same storage can be reused safely. */
void umi_test_suite_destroy(UmiTestSuite *suite)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (suite != NULL) {
        free(suite->cases);
        free(suite);
    }
}

/* Add test suite only after its inputs and available capacity have been checked. */
UmiStatus umi_test_suite_add(UmiTestSuite *suite,
                             const UmiTestCase *test_case)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (suite == NULL || test_case == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (suite->count >= UMI_TEST_SUITE_MAX_CASES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_test_suite_find(suite, test_case->test_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    suite->cases[suite->count++] = *test_case;
    return UMI_STATUS_OK;
}

/* Return the number of records represented by test suite without changing their state. */
size_t umi_test_suite_count(const UmiTestSuite *suite)
{
    return suite != NULL ? suite->count : 0U;
}

/* Find test suite while leaving the underlying catalogue or model owned by this module. */
const UmiTestCase *umi_test_suite_at(const UmiTestSuite *suite,
                                     size_t index)
{
    return suite != NULL && index < suite->count
        ? &suite->cases[index]
        : NULL;
}

/* Find test suite while leaving the underlying catalogue or model owned by this module. */
const UmiTestCase *umi_test_suite_find(const UmiTestSuite *suite,
                                       const char *test_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (suite == NULL || test_id == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < suite->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(suite->cases[index].test_id, test_id) == 0) {
            return &suite->cases[index];
        }
    }
    return NULL;
}

/* Provide the test suite id operation used by this module and its client applications. */
const char *umi_test_suite_id(const UmiTestSuite *suite)
{
    return suite != NULL ? suite->suite_id : NULL;
}

/* Provide the test suite name operation used by this module and its client applications. */
const char *umi_test_suite_name(const UmiTestSuite *suite)
{
    return suite != NULL ? suite->name : NULL;
}
