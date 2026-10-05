/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_workspace_profile.h
 *
 * PURPOSE:
 *   Model test workspace profile state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_WORKSPACE_PROFILE_H
#define UMICOM_TEST_WORKBENCH_TEST_WORKSPACE_PROFILE_H
#include "umicom/test/workbench/workbench_types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test workspace profile data shared with callers of this public contract.
 */
typedef struct UmiTestWorkspaceProfile {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestWorkspaceProfile;
/**
 * Initialise test workspace profile from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_workspace_profile_init(UmiTestWorkspaceProfile *model,const char *id,const char *label);
/**
 * Exercise test workspace profile set active and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_test_workspace_profile_set_active(UmiTestWorkspaceProfile *model,bool active);
/**
 * Return the number of records represented by test workspace profile set without changing
 * their state.
 */
UmiStatus umi_test_workspace_profile_set_count(UmiTestWorkspaceProfile *model,uint32_t item_count);
/**
 * Exercise test workspace profile set state and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_test_workspace_profile_set_state(UmiTestWorkspaceProfile *model,UmiTestWorkbenchState state);
/**
 * Check that test workspace profile satisfies its contract before another service relies
 * on it.
 */
int umi_test_workspace_profile_valid(const UmiTestWorkspaceProfile *model);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_test_workspace_profile_archive_encode(const UmiTestWorkspaceProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_workspace_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiTestWorkspaceProfile *value);

#ifdef __cplusplus
}
#endif
#endif
