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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_TARGET_SIZE_CERTIFICATION_H
#define UMICOM_UI_APPEARANCE_TARGET_SIZE_CERTIFICATION_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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

#ifdef __cplusplus
}
#endif
#endif
