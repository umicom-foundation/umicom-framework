/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/catalogue.c
 *
 * PURPOSE:
 *   Validate every suite experience and derive layout coverage without hard-coded per-application duplication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/suite_layout/catalogue.h"

#include <string.h>
#include "umicom/application/experience_catalogue.h"

/* Build one suite-wide layout index from the authoritative experience
 * catalogue; this function never creates application-owned layout copies. */
UmiStatus umi_application_suite_layout_catalogue_build(
    UmiApplicationSuiteLayoutCatalogue *out_catalogue)
{
    size_t index;
    UmiStatus status;
    /* The caller owns the catalogue storage, so a missing destination cannot
     * be replaced with hidden allocation. */
    if (out_catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_catalogue, 0, sizeof(*out_catalogue));
    status = umi_application_experience_catalogue_validate();
    /* Suite layout facts are meaningful only when the source catalogue has
     * valid application, panel and layout relationships. */
    if (status != UMI_STATUS_OK) return status;
    /* The public catalogue uses bounded storage. This guard prevents a future
     * registry expansion from writing past the application array. */
    if (umi_application_experience_catalogue_count() >
        UMI_APPLICATION_EXPERIENCE_REGISTRY_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Build each application through the same descriptor path so no product
     * receives private validation rules. */
    for (index = 0U; index < umi_application_experience_catalogue_count(); ++index) {
        UmiApplicationSuiteLayoutDescriptor *descriptor =
            &out_catalogue->applications[out_catalogue->count];
        status = umi_application_suite_layout_descriptor_build(
            umi_application_experience_catalogue_at(index), descriptor);
        /* Stop at the first incomplete application and preserve its exact
         * validation status for the caller. */
        if (status != UMI_STATUS_OK) return status;
        out_catalogue->count++;
        /* Aggregate layout behaviour so launchers and documentation tools can
         * explain suite-wide customisation without scanning application code. */
        out_catalogue->layout_count += descriptor->layout_count;
        out_catalogue->lockable_layout_count +=
            descriptor->lockable_layout_count;
        out_catalogue->multi_monitor_layout_count +=
            descriptor->multi_monitor_layout_count;
        out_catalogue->responsive_layout_count +=
            descriptor->responsive_layout_count;
        out_catalogue->context_linked_layout_count +=
            descriptor->context_linked_layout_count;
        /* Separate ready and incomplete applications for release reporting. */
        if (descriptor->complete) out_catalogue->complete_count++;
        else out_catalogue->incomplete_count++;
    }
    return out_catalogue->incomplete_count == 0U
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/* Find one borrowed application descriptor by its stable application ID. */
const UmiApplicationSuiteLayoutDescriptor *
umi_application_suite_layout_catalogue_find(
    const UmiApplicationSuiteLayoutCatalogue *catalogue,
    const char *application_id)
{
    size_t index;
    /* A search without a catalogue or identifier has no valid result. */
    if (catalogue == NULL || application_id == NULL) return NULL;
    /* Search only the descriptors that were built, not unused capacity. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Stable application identifiers are the shared key used by launchers,
         * saved layouts and thin application modules. */
        if (strcmp(catalogue->applications[index].application_id,
                   application_id) == 0)
            return &catalogue->applications[index];
    }
    return NULL;
}
