/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/contrast_certification.c
 *
 * PURPOSE:
 *   Certify measured Design-System contrast ratios against policy thresholds without duplicating colour science.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/contrast_certification.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_contrast_certification_init(UmiAppearanceContrastCertification *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->target_id,sizeof item->target_id,"text.primary");
    item->measured_ratio=7.0;
    item->required_ratio=4.5;
    item->passed=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_contrast_certification_is_valid(const UmiAppearanceContrastCertification *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->target_id) && item->measured_ratio >= 1.0 && item->required_ratio >= 1.0);
}
/*
 * Provide the appearance contrast certification evaluate operation used by this module and
 * its client applications.
 */
UmiStatus umi_appearance_contrast_certification_evaluate(UmiAppearanceContrastCertification *item,double measured,double required){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||measured<1.0||required<1.0)return UMI_STATUS_INVALID_ARGUMENT;item->measured_ratio=measured;item->required_ratio=required;item->passed=measured>=required;return UMI_STATUS_OK;}
