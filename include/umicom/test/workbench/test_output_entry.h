/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_output_entry.h
 *
 * PURPOSE:
 *   Model test output entry state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_OUTPUT_ENTRY_H
#define UMICOM_TEST_WORKBENCH_TEST_OUTPUT_ENTRY_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test output entry data shared with callers of this public contract.
 */
typedef struct UmiTestOutputEntry {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestOutputEntry;
/**
 * Initialise test output entry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_output_entry_init(UmiTestOutputEntry *model,const char *id,const char *label);
/**
 * Exercise test output entry set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_output_entry_set_active(UmiTestOutputEntry *model,bool active);
/**
 * Return the number of records represented by test output entry set without changing their
 * state.
 */
UmiStatus umi_test_output_entry_set_count(UmiTestOutputEntry *model,uint32_t item_count);
/**
 * Exercise test output entry set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_output_entry_set_state(UmiTestOutputEntry *model,UmiTestWorkbenchState state);
/**
 * Check that test output entry satisfies its contract before another service relies on it.
 */
int umi_test_output_entry_valid(const UmiTestOutputEntry *model);
#ifdef __cplusplus
}
#endif
#endif
