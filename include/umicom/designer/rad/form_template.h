/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/form_template.h
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
#ifndef UMICOM_DESIGNER_RAD_FORM_TEMPLATE_H
#define UMICOM_DESIGNER_RAD_FORM_TEMPLATE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad form template data shared with callers of this public contract.
 */
typedef struct UmiRadFormTemplate {
    char template_id[UMI_RAD_ID_CAPACITY];
    char name[UMI_RAD_TEXT_CAPACITY];
    size_t field_count;
    size_t action_count;
} UmiRadFormTemplate;
/**
 * Initialise rad form template from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_form_template_init(UmiRadFormTemplate *item);
/**
 * Check that rad form template satisfies its contract before another service relies on it.
 */
int umi_rad_form_template_is_valid(const UmiRadFormTemplate *item);
#ifdef __cplusplus
}
#endif
#endif
