/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/property_editor_descriptor.h
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
#ifndef UMICOM_DESIGNER_RAD_PROPERTY_EDITOR_DESCRIPTOR_H
#define UMICOM_DESIGNER_RAD_PROPERTY_EDITOR_DESCRIPTOR_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadPropertyEditorDescriptor {
    char property_id[UMI_RAD_ID_CAPACITY];
    char editor_type[UMI_RAD_ID_CAPACITY];
    char value_type[UMI_RAD_ID_CAPACITY];
    bool required;
    bool read_only;
} UmiRadPropertyEditorDescriptor;
UmiStatus umi_rad_property_editor_descriptor_init(UmiRadPropertyEditorDescriptor *item);
int umi_rad_property_editor_descriptor_is_valid(const UmiRadPropertyEditorDescriptor *item);
#ifdef __cplusplus
}
#endif
#endif
