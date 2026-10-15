/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/revision_cases.h
 * PURPOSE: Check that exhausted revision tokens cannot publish another mutation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_REVISION_CASES_H
#define UMICOM_TEST_REVISION_CASES_H
#include "umicom/base/status.h"
#include <stdint.h>
#include "fixture_assertions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Invoke an already prepared domain operation at the final revision token.
 * The checkpoint is heap-owned because some workbench values are large. Restore
 * the fixture after the check so its existing behavioural assertions still run.
 * Failure terminates the test even in builds that disable the C assert macro. */
#define UMI_TEST_REVISION_EXHAUSTION(Type, Address, Operation) do { \
    Type *umi_revision_case_value = (Address); \
    Type *umi_revision_case_before = malloc(sizeof(*umi_revision_case_before)); \
    if (umi_revision_case_before == NULL) { \
        fprintf(stderr, "%s:%d: cannot allocate revision checkpoint\n", __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
    uint64_t umi_revision_case_token = umi_revision_case_value->revision; \
    umi_revision_case_value->revision = UINT64_MAX; \
    memcpy(umi_revision_case_before, umi_revision_case_value, sizeof(*umi_revision_case_before)); \
    UmiStatus umi_revision_case_status = (Operation); \
    int umi_revision_case_ok = umi_revision_case_status == UMI_STATUS_CAPACITY_EXCEEDED && \
        memcmp(umi_revision_case_before, umi_revision_case_value, sizeof(*umi_revision_case_before)) == 0; \
    memcpy(umi_revision_case_value, umi_revision_case_before, sizeof(*umi_revision_case_before)); \
    umi_revision_case_value->revision = umi_revision_case_token; \
    free(umi_revision_case_before); \
    if (!umi_revision_case_ok) { \
        fprintf(stderr, "%s:%d: exhausted revision changed state or returned an unexpected status\n", \
            __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)
#endif
