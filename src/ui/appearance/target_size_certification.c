/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/target_size_certification.c
 *
 * PURPOSE:
 *   Certify resolved interactive target dimensions against modality-specific accessibility policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/target_size_certification.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_target_size_certification_init(UmiAppearanceTargetSizeCertification *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->target_id,sizeof item->target_id,"button.submit");
    item->width_dp=44.0;
    item->height_dp=44.0;
    item->required_width_dp=44.0;
    item->required_height_dp=44.0;
    item->passed=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_target_size_certification_is_valid(const UmiAppearanceTargetSizeCertification *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->target_id) && item->width_dp > 0.0 && item->height_dp > 0.0);
}
/*
 * Provide the appearance target size certification evaluate operation used by this module
 * and its client applications.
 */
void umi_appearance_target_size_certification_evaluate(UmiAppearanceTargetSizeCertification *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item!=NULL)item->passed=item->width_dp>=item->required_width_dp&&item->height_dp>=item->required_height_dp;}
