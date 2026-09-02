/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/page_template.h
 *
 * PURPOSE:
 *   Describe reusable page templates without embedding application-specific logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_PAGE_TEMPLATE_H
#define UMICOM_DESIGNER_RAD_PAGE_TEMPLATE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad page template data shared with callers of this public contract.
 */
typedef struct UmiRadPageTemplate {
    char template_id[UMI_RAD_ID_CAPACITY];
    char name[UMI_RAD_TEXT_CAPACITY];
    char shell_kind[UMI_RAD_ID_CAPACITY];
    size_t initial_components;
} UmiRadPageTemplate;
/**
 * Initialise rad page template from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_page_template_init(UmiRadPageTemplate *item);
/**
 * Check that rad page template satisfies its contract before another service relies on it.
 */
int umi_rad_page_template_is_valid(const UmiRadPageTemplate *item);
#ifdef __cplusplus
}
#endif
#endif
