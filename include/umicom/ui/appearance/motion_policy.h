/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/motion_policy.h
 *
 * PURPOSE:
 *   Define semantic motion allowances and maximum transition durations for production UI.
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
#ifndef UMICOM_UI_APPEARANCE_MOTION_POLICY_H
#define UMICOM_UI_APPEARANCE_MOTION_POLICY_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance motion policy data shared with callers of this public contract.
 */
typedef struct UmiAppearanceMotionPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    uint32_t standard_duration_ms;
    uint32_t emphasis_duration_ms;
    bool allow_decorative_motion;
    bool allow_parallax;
} UmiAppearanceMotionPolicy;

/* Initialise one motion policy record with deterministic defaults. */
UmiStatus umi_appearance_motion_policy_init(UmiAppearanceMotionPolicy *item);
/* Validate the required production invariants for this motion policy. */
int umi_appearance_motion_policy_is_valid(const UmiAppearanceMotionPolicy *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_motion_policy_archive_encode(const UmiAppearanceMotionPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_motion_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceMotionPolicy *value);

#ifdef __cplusplus
}
#endif
#endif
