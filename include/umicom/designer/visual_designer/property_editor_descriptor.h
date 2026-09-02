/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/property_editor_descriptor.h
 *
 * PURPOSE:
 *   Describe an editor choice for a semantic component property.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_PROPERTY_EDITOR_DESCRIPTOR_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_PROPERTY_EDITOR_DESCRIPTOR_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer property editor descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiRadPropertyEditorDescriptor {
    char property_id[UMI_RAD_ID_CAPACITY];
    char editor_type[UMI_RAD_ID_CAPACITY];
    char value_type[UMI_RAD_ID_CAPACITY];
    bool required;
    bool read_only;
} UmiRadPropertyEditorDescriptor;
/**
 * Initialise visual designer property editor descriptor from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_rad_property_editor_descriptor_init(UmiRadPropertyEditorDescriptor *item);
/**
 * Check that visual designer property editor descriptor satisfies its contract before another service
 * relies on it.
 */
int umi_rad_property_editor_descriptor_is_valid(const UmiRadPropertyEditorDescriptor *item);
#ifdef __cplusplus
}
#endif
#endif
