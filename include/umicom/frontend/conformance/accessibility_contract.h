/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/accessibility_contract.h
 *
 * PURPOSE:
 *   required semantic accessibility roles, names, states and keyboard affordances.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_ACCESSIBILITY_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_ACCESSIBILITY_CONTRACT_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc accessibility contract data shared with callers of this public
 * contract.
 */
typedef struct UmiFcAccessibilityContract { uint64_t required_roles; bool named; bool keyboard_reachable; bool state_exposed; } UmiFcAccessibilityContract;
/**
 * Check that fc accessibility contract satisfies its contract before another service
 * relies on it.
 */
bool umi_fc_accessibility_contract_validate(const UmiFcAccessibilityContract *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fc_accessibility_contract_archive_encode(const UmiFcAccessibilityContract *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fc_accessibility_contract_archive_decode(const void *bytes, size_t byte_count,
    UmiFcAccessibilityContract *value);

#ifdef __cplusplus
}
#endif
#endif
