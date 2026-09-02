/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/workbench_types.h
 *
 * PURPOSE:
 *   Define bounded identifiers, text, states and common Test/Quality workbench records.
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
#ifndef UMICOM_TEST_WORKBENCH_TYPES_H
#define UMICOM_TEST_WORKBENCH_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TEST_WORKBENCH_ID_CAPACITY 128U
#define UMI_TEST_WORKBENCH_TEXT_CAPACITY 256U
#define UMI_TEST_WORKBENCH_DETAIL_CAPACITY 384U
#define UMI_TEST_WORKBENCH_MAX_ITEMS 128U
/**
 * List the named test workbench state values accepted by this public contract.
 */
typedef enum UmiTestWorkbenchState {
    UMI_TEST_WORKBENCH_STATE_IDLE = 0,
    UMI_TEST_WORKBENCH_STATE_DISCOVERING = 1,
    UMI_TEST_WORKBENCH_STATE_READY = 2,
    UMI_TEST_WORKBENCH_STATE_RUNNING = 3,
    UMI_TEST_WORKBENCH_STATE_PASSED = 4,
    UMI_TEST_WORKBENCH_STATE_FAILED = 5,
    UMI_TEST_WORKBENCH_STATE_SKIPPED = 6,
    UMI_TEST_WORKBENCH_STATE_CANCELLED = 7
} UmiTestWorkbenchState;
/**
 * Represent the test workbench entry data shared with callers of this public contract.
 */
typedef struct UmiTestWorkbenchEntry {
    char id[UMI_TEST_WORKBENCH_ID_CAPACITY];
    char label[UMI_TEST_WORKBENCH_TEXT_CAPACITY];
    char detail[UMI_TEST_WORKBENCH_DETAIL_CAPACITY];
    UmiTestWorkbenchState state;
    uint32_t flags;
    uint32_t score;
    uint64_t duration_us;
    uint64_t revision;
} UmiTestWorkbenchEntry;
/**
 * Exercise test workbench copy text and return a clear result when the behaviour no longer
 * matches its contract.
 */
UmiStatus umi_test_workbench_copy_text(char *destination,size_t capacity,const char *source);
/**
 * Initialise test workbench entry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_test_workbench_entry_init(UmiTestWorkbenchEntry *entry,const char *id,const char *label);
/**
 * Exercise test workbench entry set detail and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_workbench_entry_set_detail(UmiTestWorkbenchEntry *entry,const char *detail);
/**
 * Exercise test workbench entry set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_workbench_entry_set_state(UmiTestWorkbenchEntry *entry,UmiTestWorkbenchState state);
/**
 * Check that test workbench entry satisfies its contract before another service relies on
 * it.
 */
int umi_test_workbench_entry_valid(const UmiTestWorkbenchEntry *entry);
#ifdef __cplusplus
}
#endif
#endif
