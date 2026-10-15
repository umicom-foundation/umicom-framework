/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/install_state.h
 *
 * PURPOSE:
 *   installed application version, channel and health state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_INSTALL_STATE_H
#define UMICOM_DISTRIBUTION_RUNTIME_INSTALL_STATE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr install state data shared with callers of this public contract.
 */
typedef struct UmiDrInstallState { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; UmiDrVersion version; UmiDrChannelKind channel; UmiDrInstallScope scope; bool healthy; } UmiDrInstallState;
/**
 * Initialise dr install state from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_install_state_init(UmiDrInstallState *value);
/**
 * Check that dr install state satisfies its contract before another service relies on it.
 */
bool umi_dr_install_state_valid(const UmiDrInstallState *value);
/**
 * Provide the dr install state fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_install_state_fingerprint(const UmiDrInstallState *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_install_state_archive_encode(const UmiDrInstallState *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_install_state_archive_decode(const void *bytes, size_t byte_count,
    UmiDrInstallState *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_dr_install_state_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDrInstallState *expected, UmiDrInstallState *value);


#ifdef __cplusplus
}
#endif
#endif
