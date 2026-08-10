/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/suite.c
 *
 * PURPOSE:
 *   Implement bounded test-suite ownership and lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

UmiStatus umi_test_suite_create(const char *suite_id,
                                const char *name,
                                UmiTestSuite **out_suite)
{
    UmiTestSuite *suite;
    if (suite_id == NULL || name == NULL || out_suite == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_suite = NULL;
    suite = (UmiTestSuite *)calloc(1U, sizeof(*suite));
    if (suite == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    suite->cases = (UmiTestCase *)calloc(UMI_TEST_SUITE_MAX_CASES,
                                         sizeof(*suite->cases));
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

void umi_test_suite_destroy(UmiTestSuite *suite)
{
    if (suite != NULL) {
        free(suite->cases);
        free(suite);
    }
}

UmiStatus umi_test_suite_add(UmiTestSuite *suite,
                             const UmiTestCase *test_case)
{
    if (suite == NULL || test_case == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (suite->count >= UMI_TEST_SUITE_MAX_CASES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_test_suite_find(suite, test_case->test_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    suite->cases[suite->count++] = *test_case;
    return UMI_STATUS_OK;
}

size_t umi_test_suite_count(const UmiTestSuite *suite)
{
    return suite != NULL ? suite->count : 0U;
}

const UmiTestCase *umi_test_suite_at(const UmiTestSuite *suite,
                                     size_t index)
{
    return suite != NULL && index < suite->count
        ? &suite->cases[index]
        : NULL;
}

const UmiTestCase *umi_test_suite_find(const UmiTestSuite *suite,
                                       const char *test_id)
{
    size_t index;
    if (suite == NULL || test_id == NULL) {
        return NULL;
    }
    for (index = 0U; index < suite->count; ++index) {
        if (strcmp(suite->cases[index].test_id, test_id) == 0) {
            return &suite->cases[index];
        }
    }
    return NULL;
}

const char *umi_test_suite_id(const UmiTestSuite *suite)
{
    return suite != NULL ? suite->suite_id : NULL;
}

const char *umi_test_suite_name(const UmiTestSuite *suite)
{
    return suite != NULL ? suite->name : NULL;
}
