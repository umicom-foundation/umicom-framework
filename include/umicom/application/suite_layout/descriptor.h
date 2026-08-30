/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/descriptor.h
 *
 * PURPOSE:
 *   Describe layout coverage for one application from the canonical experience catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_DESCRIPTOR_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_DESCRIPTOR_H

#include "umicom/application/experience.h"
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationSuiteLayoutDescriptor {
    char application_id[UMI_UI_ID_CAPACITY];
    char display_name[UMI_UI_TEXT_CAPACITY];
    char default_layout_id[UMI_UI_ID_CAPACITY];
    size_t panel_count;
    size_t layout_count;
    size_t default_panel_count;
    size_t left_count;
    size_t centre_count;
    size_t right_count;
    size_t bottom_count;
    size_t top_count;
    size_t floating_count;
    int default_layout_valid;
    int complete;
} UmiApplicationSuiteLayoutDescriptor;

UmiStatus umi_application_suite_layout_descriptor_build(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationSuiteLayoutDescriptor *out_descriptor);

#ifdef __cplusplus
}
#endif
#endif
