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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_CONTRAST_CERTIFICATION_H
#define UMICOM_UI_APPEARANCE_CONTRAST_CERTIFICATION_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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

#ifdef __cplusplus
}
#endif
#endif
