/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/form_descriptor.h
 *
 * PURPOSE:
 *   Describe a form, semantic root and Framework submit command.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_FORM_DESCRIPTOR_H
#define UMICOM_DESIGNER_RAD_FORM_DESCRIPTOR_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadFormDescriptor {
    char form_id[UMI_RAD_ID_CAPACITY];
    char title[UMI_RAD_TEXT_CAPACITY];
    char root_component_id[UMI_RAD_ID_CAPACITY];
    char submit_command_id[UMI_RAD_ID_CAPACITY];
} UmiRadFormDescriptor;
UmiStatus umi_rad_form_descriptor_init(UmiRadFormDescriptor *item);
int umi_rad_form_descriptor_is_valid(const UmiRadFormDescriptor *item);
#ifdef __cplusplus
}
#endif
#endif
