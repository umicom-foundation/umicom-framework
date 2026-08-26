/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/page_template.h
 *
 * PURPOSE:
 *   Describe reusable page templates without embedding application-specific logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_PAGE_TEMPLATE_H
#define UMICOM_DESIGNER_RAD_PAGE_TEMPLATE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadPageTemplate {
    char template_id[UMI_RAD_ID_CAPACITY];
    char name[UMI_RAD_TEXT_CAPACITY];
    char shell_kind[UMI_RAD_ID_CAPACITY];
    size_t initial_components;
} UmiRadPageTemplate;
UmiStatus umi_rad_page_template_init(UmiRadPageTemplate *item);
int umi_rad_page_template_is_valid(const UmiRadPageTemplate *item);
#ifdef __cplusplus
}
#endif
#endif
