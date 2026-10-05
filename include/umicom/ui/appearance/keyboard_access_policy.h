/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/keyboard_access_policy.h
 *
 * PURPOSE:
 *   Define keyboard operability and transient-surface escape requirements for all applications.
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
#ifndef UMICOM_UI_APPEARANCE_KEYBOARD_ACCESS_POLICY_H
#define UMICOM_UI_APPEARANCE_KEYBOARD_ACCESS_POLICY_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance keyboard access policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceKeyboardAccessPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    bool require_all_interactive;
    bool require_escape_transients;
    bool require_arrow_navigation;
    bool forbid_keyboard_traps;
} UmiAppearanceKeyboardAccessPolicy;

/* Initialise one keyboard access policy record with deterministic defaults. */
UmiStatus umi_appearance_keyboard_access_policy_init(UmiAppearanceKeyboardAccessPolicy *item);
/* Validate the required production invariants for this keyboard access policy. */
int umi_appearance_keyboard_access_policy_is_valid(const UmiAppearanceKeyboardAccessPolicy *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_keyboard_access_policy_archive_encode(const UmiAppearanceKeyboardAccessPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_keyboard_access_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceKeyboardAccessPolicy *value);

#ifdef __cplusplus
}
#endif
#endif
