/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_output_session.h
 *
 * PURPOSE:
 *   Model test output session state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_OUTPUT_SESSION_H
#define UMICOM_TEST_WORKBENCH_TEST_OUTPUT_SESSION_H
#include "umicom/test/workbench/workbench_types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test output session data shared with callers of this public contract.
 */
typedef struct UmiTestOutputSession {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestOutputSession;
/**
 * Initialise test output session from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_output_session_init(UmiTestOutputSession *model,const char *id,const char *label);
/**
 * Exercise test output session set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_output_session_set_active(UmiTestOutputSession *model,bool active);
/**
 * Return the number of records represented by test output session set without changing
 * their state.
 */
UmiStatus umi_test_output_session_set_count(UmiTestOutputSession *model,uint32_t item_count);
/**
 * Exercise test output session set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_output_session_set_state(UmiTestOutputSession *model,UmiTestWorkbenchState state);
/**
 * Check that test output session satisfies its contract before another service relies on
 * it.
 */
int umi_test_output_session_valid(const UmiTestOutputSession *model);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_test_output_session_archive_encode(const UmiTestOutputSession *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_output_session_archive_decode(const void *bytes, size_t byte_count,
    UmiTestOutputSession *value);

#ifdef __cplusplus
}
#endif
#endif
