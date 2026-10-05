/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/system_appearance_state.h
 *
 * PURPOSE:
 *   Represent operating-system appearance signals without coupling Framework logic to platform APIs.
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
#ifndef UMICOM_UI_APPEARANCE_SYSTEM_APPEARANCE_STATE_H
#define UMICOM_UI_APPEARANCE_SYSTEM_APPEARANCE_STATE_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance system appearance state data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceSystemAppearanceState {
    char system_id[UMI_APPEARANCE_ID_CAPACITY];
    bool dark_mode;
    bool high_contrast;
    bool reduced_motion;
    uint32_t dpi;
    double scale;
} UmiAppearanceSystemAppearanceState;

/* Initialise one system appearance state record with deterministic defaults. */
UmiStatus umi_appearance_system_appearance_state_init(UmiAppearanceSystemAppearanceState *item);
/* Validate the required production invariants for this system appearance state. */
int umi_appearance_system_appearance_state_is_valid(const UmiAppearanceSystemAppearanceState *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_system_appearance_state_archive_encode(const UmiAppearanceSystemAppearanceState *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_system_appearance_state_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceSystemAppearanceState *value);

#ifdef __cplusplus
}
#endif
#endif
