/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/form_template.h
 *
 * PURPOSE:
 *   Describe reusable form templates and expected field/action counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_FORM_TEMPLATE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_FORM_TEMPLATE_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer form template data shared with callers of this public contract.
 */
typedef struct UmiRadFormTemplate {
    char template_id[UMI_RAD_ID_CAPACITY];
    char name[UMI_RAD_TEXT_CAPACITY];
    size_t field_count;
    size_t action_count;
} UmiRadFormTemplate;
/**
 * Initialise visual designer form template from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_form_template_init(UmiRadFormTemplate *item);
/**
 * Check that visual designer form template satisfies its contract before another service relies on it.
 */
int umi_rad_form_template_is_valid(const UmiRadFormTemplate *item);
#ifdef __cplusplus
}
#endif
#endif
