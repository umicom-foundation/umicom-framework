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

typedef struct UmiApplicationSuiteLayoutCatalogue {
    UmiApplicationSuiteLayoutDescriptor
        applications[UMI_APPLICATION_EXPERIENCE_REGISTRY_CAPACITY];
    size_t count;
    size_t complete_count;
    size_t incomplete_count;
} UmiApplicationSuiteLayoutCatalogue;

UmiStatus umi_application_suite_layout_catalogue_build(
    UmiApplicationSuiteLayoutCatalogue *out_catalogue);
const UmiApplicationSuiteLayoutDescriptor *
umi_application_suite_layout_catalogue_find(
    const UmiApplicationSuiteLayoutCatalogue *catalogue,
    const char *application_id);

#ifdef __cplusplus
}
#endif
#endif
