/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/page_descriptor.h
 *
 * PURPOSE:
 *   Describe a visual application page, route and semantic root component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_PAGE_DESCRIPTOR_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_PAGE_DESCRIPTOR_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer page descriptor data shared with callers of this public contract.
 */
typedef struct UmiRadPageDescriptor {
    char page_id[UMI_RAD_ID_CAPACITY];
    char route[UMI_RAD_PATH_CAPACITY];
    char title[UMI_RAD_TEXT_CAPACITY];
    char root_component_id[UMI_RAD_ID_CAPACITY];
} UmiRadPageDescriptor;
/**
 * Initialise visual designer page descriptor from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_page_descriptor_init(UmiRadPageDescriptor *item);
/**
 * Check that visual designer page descriptor satisfies its contract before another service relies on
 * it.
 */
int umi_rad_page_descriptor_is_valid(const UmiRadPageDescriptor *item);
#ifdef __cplusplus
}
#endif
#endif
