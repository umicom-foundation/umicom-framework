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
typedef struct UmiRadFormTemplate {
    char template_id[UMI_RAD_ID_CAPACITY];
    char name[UMI_RAD_TEXT_CAPACITY];
    size_t field_count;
    size_t action_count;
} UmiRadFormTemplate;
UmiStatus umi_rad_form_template_init(UmiRadFormTemplate *item);
int umi_rad_form_template_is_valid(const UmiRadFormTemplate *item);
#ifdef __cplusplus
}
#endif
#endif
