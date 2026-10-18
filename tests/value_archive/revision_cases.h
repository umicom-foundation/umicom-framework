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

/* Exercise an individual 64-bit counter at both final usable states. The
 * operation must already have valid arguments and no external side effects.
 * Restoring every byte lets each nested counter be tested independently, so a
 * guard on the outer revision cannot conceal a missing nested-value guard. */
#define UMI_TEST_VALUE_COUNTER_BOUNDARY(Type, Address, Field, Operation) do { \
    Type *umi_counter_value = (Address); \
    Type *umi_counter_before = malloc(sizeof(*umi_counter_before)); \
    Type *umi_counter_exhausted = malloc(sizeof(*umi_counter_exhausted)); \
    if (umi_counter_before == NULL || umi_counter_exhausted == NULL) { \
        free(umi_counter_before); \
        free(umi_counter_exhausted); \
        fprintf(stderr, "%s:%d: cannot allocate counter checkpoints\n", __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
    memcpy(umi_counter_before, umi_counter_value, sizeof(*umi_counter_before)); \
    umi_counter_value->Field = UINT64_MAX - UINT64_C(1); \
    UmiStatus umi_counter_status = (Operation); \
    int umi_counter_ok = umi_counter_status == UMI_STATUS_OK && \
        umi_counter_value->Field == UINT64_MAX; \
    memcpy(umi_counter_value, umi_counter_before, sizeof(*umi_counter_before)); \
    umi_counter_value->Field = UINT64_MAX; \
    memcpy(umi_counter_exhausted, umi_counter_value, sizeof(*umi_counter_exhausted)); \
    umi_counter_status = (Operation); \
    umi_counter_ok = umi_counter_ok && umi_counter_status == UMI_STATUS_CAPACITY_EXCEEDED && \
        memcmp(umi_counter_exhausted, umi_counter_value, sizeof(*umi_counter_exhausted)) == 0; \
    memcpy(umi_counter_value, umi_counter_before, sizeof(*umi_counter_before)); \
    free(umi_counter_exhausted); \
    free(umi_counter_before); \
    if (!umi_counter_ok) { \
        fprintf(stderr, "%s:%d: %s boundary changed state or returned an unexpected status\n", \
            __FILE__, __LINE__, #Field); \
        exit(EXIT_FAILURE); \
    } \
} while (0)
#endif
