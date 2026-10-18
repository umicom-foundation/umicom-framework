/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/configuration_layout.h
 *
 * PURPOSE:
 *   system, user and portable configuration-root policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_CONFIGURATION_LAYOUT_H
#define UMICOM_DISTRIBUTION_RUNTIME_CONFIGURATION_LAYOUT_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr configuration layout data shared with callers of this public contract.
 */
typedef struct UmiDrConfigurationLayout { char id[UMI_DR_ID_CAPACITY]; char system_root[UMI_DR_PATH_CAPACITY]; char user_root[UMI_DR_PATH_CAPACITY]; bool portable; } UmiDrConfigurationLayout;
/**
 * Initialise dr configuration layout from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_configuration_layout_init(UmiDrConfigurationLayout *value);
/**
 * Check that dr configuration layout satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_configuration_layout_valid(const UmiDrConfigurationLayout *value);
/**
 * Provide the dr configuration layout fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_configuration_layout_fingerprint(const UmiDrConfigurationLayout *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_configuration_layout_archive_encode(const UmiDrConfigurationLayout *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_configuration_layout_archive_decode(const void *bytes, size_t byte_count,
    UmiDrConfigurationLayout *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiDrConfigurationLayout object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_dr_configuration_layout_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiDrConfigurationLayout *workspace, UmiDrConfigurationLayout *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_dr_configuration_layout_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDrConfigurationLayout *expected, UmiDrConfigurationLayout *value);


#ifdef __cplusplus
}
#endif
#endif
