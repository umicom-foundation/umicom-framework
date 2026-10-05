/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/mnemonic_policy.h
 *
 * PURPOSE:
 *   Govern mnemonic visibility and uniqueness without embedding toolkit accelerator syntax.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_MNEMONIC_POLICY_H
#define UMICOM_UI_APPEARANCE_MNEMONIC_POLICY_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance mnemonic policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceMnemonicPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    bool show_on_keyboard_intent;
    bool unique_within_scope;
    bool localised;
    bool allow_auto_assignment;
} UmiAppearanceMnemonicPolicy;

/* Initialise one mnemonic policy record with deterministic defaults. */
UmiStatus umi_appearance_mnemonic_policy_init(UmiAppearanceMnemonicPolicy *item);
/* Validate the required production invariants for this mnemonic policy. */
int umi_appearance_mnemonic_policy_is_valid(const UmiAppearanceMnemonicPolicy *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_mnemonic_policy_archive_encode(const UmiAppearanceMnemonicPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_mnemonic_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceMnemonicPolicy *value);

#ifdef __cplusplus
}
#endif
#endif
