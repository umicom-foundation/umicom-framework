/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/launch_profile.h
 *
 * PURPOSE:
 *   named launch profile with environment, frontend and safe-mode controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_LAUNCH_PROFILE_H
#define UMICOM_DISTRIBUTION_RUNTIME_LAUNCH_PROFILE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr launch profile data shared with callers of this public contract.
 */
typedef struct UmiDrLaunchProfile { char id[UMI_DR_ID_CAPACITY]; char launcher_id[UMI_DR_ID_CAPACITY]; char environment_id[UMI_DR_ID_CAPACITY]; char frontend[32]; bool safe_mode; bool offline; } UmiDrLaunchProfile;
/**
 * Initialise dr launch profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_launch_profile_init(UmiDrLaunchProfile *value);
/**
 * Check that dr launch profile satisfies its contract before another service relies on it.
 */
bool umi_dr_launch_profile_valid(const UmiDrLaunchProfile *value);
/**
 * Provide the dr launch profile fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_launch_profile_fingerprint(const UmiDrLaunchProfile *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_launch_profile_archive_encode(const UmiDrLaunchProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_launch_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiDrLaunchProfile *value);

#ifdef __cplusplus
}
#endif
#endif
