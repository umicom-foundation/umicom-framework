/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/keyboard_contract.h
 *
 * PURPOSE:
 *   required command and navigation keyboard coverage for workstation surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_KEYBOARD_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_KEYBOARD_CONTRACT_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc keyboard contract data shared with callers of this public contract.
 */
typedef struct UmiFcKeyboardContract { size_t command_count; size_t navigation_count; bool shortcuts_documented; } UmiFcKeyboardContract;
/**
 * Check that fc keyboard contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_keyboard_contract_validate(const UmiFcKeyboardContract *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fc_keyboard_contract_archive_encode(const UmiFcKeyboardContract *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fc_keyboard_contract_archive_decode(const void *bytes, size_t byte_count,
    UmiFcKeyboardContract *value);

#ifdef __cplusplus
}
#endif
#endif
