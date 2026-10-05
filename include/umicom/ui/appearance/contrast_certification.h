/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/contrast_certification.h
 *
 * PURPOSE:
 *   Certify measured Design-System contrast ratios against policy thresholds without duplicating colour science.
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
#ifndef UMICOM_UI_APPEARANCE_CONTRAST_CERTIFICATION_H
#define UMICOM_UI_APPEARANCE_CONTRAST_CERTIFICATION_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance contrast certification data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceContrastCertification {
    char target_id[UMI_APPEARANCE_ID_CAPACITY];
    double measured_ratio;
    double required_ratio;
    bool passed;
} UmiAppearanceContrastCertification;

/* Initialise one contrast certification record with deterministic defaults. */
UmiStatus umi_appearance_contrast_certification_init(UmiAppearanceContrastCertification *item);
/* Validate the required production invariants for this contrast certification. */
int umi_appearance_contrast_certification_is_valid(const UmiAppearanceContrastCertification *item);
/* Evaluate a measured contrast ratio against the resolved policy threshold. */
UmiStatus umi_appearance_contrast_certification_evaluate(UmiAppearanceContrastCertification *item,double measured,double required);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_contrast_certification_archive_encode(const UmiAppearanceContrastCertification *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_contrast_certification_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceContrastCertification *value);

#ifdef __cplusplus
}
#endif
#endif
