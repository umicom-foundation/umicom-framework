/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/catalogue.h
 *
 * PURPOSE:
 *   Publish layout-readiness coverage for every application in the canonical Umicom Application Suite.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_CATALOGUE_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_CATALOGUE_H

#include "umicom/application/suite_layout/descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Suite-wide index of application layouts and supported customisation modes.
 *
 * The catalogue owns its application descriptors. Nested layout-definition
 * pointers remain borrowed from the immutable experience catalogue.
 */
typedef struct UmiApplicationSuiteLayoutCatalogue {
    UmiApplicationSuiteLayoutDescriptor
        applications[UMI_APPLICATION_EXPERIENCE_REGISTRY_CAPACITY];
    size_t count;
    size_t layout_count;
    size_t lockable_layout_count;
    size_t multi_monitor_layout_count;
    size_t responsive_layout_count;
    size_t context_linked_layout_count;
    size_t complete_count;
    size_t incomplete_count;
} UmiApplicationSuiteLayoutCatalogue;

/**
 * Builds layout coverage for every canonical Umicom application.
 *
 * @param out_catalogue Receives application descriptors and suite totals.
 * @return `UMI_STATUS_OK` when every application and layout is complete.
 */
UmiStatus umi_application_suite_layout_catalogue_build(
    UmiApplicationSuiteLayoutCatalogue *out_catalogue);
/**
 * Finds one application descriptor by its stable application identifier.
 *
 * @param catalogue Previously built suite-wide layout catalogue.
 * @param application_id Stable identifier such as `org.umicom.studio`.
 * @return A pointer owned by `catalogue`, or `NULL` when no match exists.
 */
const UmiApplicationSuiteLayoutDescriptor *
umi_application_suite_layout_catalogue_find(
    const UmiApplicationSuiteLayoutCatalogue *catalogue,
    const char *application_id);

#ifdef __cplusplus
}
#endif
#endif
