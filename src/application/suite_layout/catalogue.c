/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/catalogue.c
 *
 * PURPOSE:
 *   Validate every suite experience and derive layout coverage without hard-coded per-application duplication.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/suite_layout/catalogue.h"

#include <string.h>
#include "umicom/application/experience_catalogue.h"

UmiStatus umi_application_suite_layout_catalogue_build(
    UmiApplicationSuiteLayoutCatalogue *out_catalogue)
{
    size_t index;
    UmiStatus status;
    if (out_catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_catalogue, 0, sizeof(*out_catalogue));
    status = umi_application_experience_catalogue_validate();
    if (status != UMI_STATUS_OK) return status;
    if (umi_application_experience_catalogue_count() >
        UMI_APPLICATION_EXPERIENCE_REGISTRY_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = 0U; index < umi_application_experience_catalogue_count(); ++index) {
        UmiApplicationSuiteLayoutDescriptor *descriptor =
            &out_catalogue->applications[out_catalogue->count];
        status = umi_application_suite_layout_descriptor_build(
            umi_application_experience_catalogue_at(index), descriptor);
        if (status != UMI_STATUS_OK) return status;
        out_catalogue->count++;
        if (descriptor->complete) out_catalogue->complete_count++;
        else out_catalogue->incomplete_count++;
    }
    return out_catalogue->incomplete_count == 0U
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

const UmiApplicationSuiteLayoutDescriptor *
umi_application_suite_layout_catalogue_find(
    const UmiApplicationSuiteLayoutCatalogue *catalogue,
    const char *application_id)
{
    size_t index;
    if (catalogue == NULL || application_id == NULL) return NULL;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->applications[index].application_id,
                   application_id) == 0)
            return &catalogue->applications[index];
    }
    return NULL;
}
