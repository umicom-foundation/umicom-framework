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

#include "umicom/application/suite_layout/layout_summary.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Combines every canonical layout summary for one application.
 *
 * The descriptor owns the summary array but each summary borrows its immutable
 * layout definition from the Framework experience catalogue.
 */
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
    UmiApplicationSuiteLayoutSummary
        layouts[UMI_APPLICATION_SUITE_LAYOUT_MAX_LAYOUTS];
    size_t summary_count;
    size_t valid_layout_count;
    size_t invalid_layout_count;
    size_t lockable_layout_count;
    size_t multi_monitor_layout_count;
    size_t responsive_layout_count;
    size_t context_linked_layout_count;
    int default_layout_valid;
    int complete;
} UmiApplicationSuiteLayoutDescriptor;

/**
 * Builds complete layout coverage for one canonical application experience.
 *
 * @param experience Borrowed application metadata from Framework.
 * @param out_descriptor Receives summaries, totals and default-layout data.
 * @return `UMI_STATUS_OK` only when every named layout is valid and complete.
 *
 * @see umi_application_suite_layout_catalogue_build
 */
UmiStatus umi_application_suite_layout_descriptor_build(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationSuiteLayoutDescriptor *out_descriptor);
/**
 * Finds one summary by the stable identifier used in saved workspaces.
 *
 * @param descriptor Previously built application descriptor.
 * @param layout_id Stable unqualified layout identifier.
 * @return A borrowed summary, or `NULL` when the layout is not present.
 */
const UmiApplicationSuiteLayoutSummary *
umi_application_suite_layout_descriptor_find_layout(
    const UmiApplicationSuiteLayoutDescriptor *descriptor,
    const char *layout_id);

#ifdef __cplusplus
}
#endif
#endif
