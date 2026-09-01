/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/layout_summary.h
 *
 * PURPOSE:
 *   Describe one canonical application layout as clear placement and
 *   customisation metadata without duplicating its panel recipe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_LAYOUT_SUMMARY_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_LAYOUT_SUMMARY_H

#include "umicom/application/experience.h"
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_SUITE_LAYOUT_MAX_LAYOUTS 32U

/**
 * Describes how one canonical layout places its panels and which forms of
 * customisation it supports.
 *
 * `definition` is borrowed from the Framework experience catalogue. It stays
 * valid for the life of the process and must not be freed or modified.
 */
typedef struct UmiApplicationSuiteLayoutSummary {
    const UmiExperienceLayoutDefinition *definition;
    size_t panel_count;
    size_t left_count;
    size_t centre_count;
    size_t right_count;
    size_t top_count;
    size_t bottom_count;
    size_t floating_count;
    int default_layout;
    int lockable;
    int multi_monitor;
    int responsive;
    int context_linked;
    int complete;
} UmiApplicationSuiteLayoutSummary;

/**
 * Validates and summarises one layout owned by an application experience.
 *
 * @param experience Canonical application experience that owns `layout`.
 * @param layout Borrowed layout definition from `experience`.
 * @param out_summary Receives placement totals and customisation flags.
 * @return `UMI_STATUS_OK` when every panel and region is valid, otherwise a
 *         status describing invalid input, missing metadata or bad state.
 *
 * No memory is allocated. The returned summary borrows its definition from
 * `experience`, so both objects follow the catalogue lifetime.
 *
 * @see umi_application_suite_layout_descriptor_build
 */
UmiStatus umi_application_suite_layout_summary_build(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperienceLayoutDefinition *layout,
    UmiApplicationSuiteLayoutSummary *out_summary);

#ifdef __cplusplus
}
#endif
#endif
