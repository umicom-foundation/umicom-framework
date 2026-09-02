/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_output_filter.h
 *
 * PURPOSE:
 *   Model test output filter state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_OUTPUT_FILTER_H
#define UMICOM_TEST_WORKBENCH_TEST_OUTPUT_FILTER_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test output filter data shared with callers of this public contract.
 */
typedef struct UmiTestOutputFilter {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestOutputFilter;
/**
 * Initialise test output filter from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_output_filter_init(UmiTestOutputFilter *model,const char *id,const char *label);
/**
 * Exercise test output filter set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_output_filter_set_active(UmiTestOutputFilter *model,bool active);
/**
 * Return the number of records represented by test output filter set without changing
 * their state.
 */
UmiStatus umi_test_output_filter_set_count(UmiTestOutputFilter *model,uint32_t item_count);
/**
 * Exercise test output filter set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_output_filter_set_state(UmiTestOutputFilter *model,UmiTestWorkbenchState state);
/**
 * Check that test output filter satisfies its contract before another service relies on
 * it.
 */
int umi_test_output_filter_valid(const UmiTestOutputFilter *model);
#ifdef __cplusplus
}
#endif
#endif
