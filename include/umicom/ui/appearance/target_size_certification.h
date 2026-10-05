/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/target_size_certification.h
 *
 * PURPOSE:
 *   Certify resolved interactive target dimensions against modality-specific accessibility policy.
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
#ifndef UMICOM_UI_APPEARANCE_TARGET_SIZE_CERTIFICATION_H
#define UMICOM_UI_APPEARANCE_TARGET_SIZE_CERTIFICATION_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance target size certification data shared with callers of this
 * public contract.
 */
typedef struct UmiAppearanceTargetSizeCertification {
    char target_id[UMI_APPEARANCE_ID_CAPACITY];
    double width_dp;
    double height_dp;
    double required_width_dp;
    double required_height_dp;
    bool passed;
} UmiAppearanceTargetSizeCertification;

/* Initialise one target size certification record with deterministic defaults. */
UmiStatus umi_appearance_target_size_certification_init(UmiAppearanceTargetSizeCertification *item);
/* Validate the required production invariants for this target size certification. */
int umi_appearance_target_size_certification_is_valid(const UmiAppearanceTargetSizeCertification *item);
/* Re-evaluate target-size compliance after adaptive layout resolution. */
void umi_appearance_target_size_certification_evaluate(UmiAppearanceTargetSizeCertification *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_target_size_certification_archive_encode(const UmiAppearanceTargetSizeCertification *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_target_size_certification_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceTargetSizeCertification *value);

#ifdef __cplusplus
}
#endif
#endif
