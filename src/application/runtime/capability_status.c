/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/capability_status.c
 *
 * PURPOSE:
 *   Build a deduplicated bounded capability status projection from product experience metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/capability_status.h"

#include <string.h>

static int has_text(const char *text)
{
    return text != NULL && text[0] != '\0';
}

static UmiStatus add_capability(
    UmiApplicationCapabilityStatus *status,
    const char *capability_id,
    UmiApplicationCapabilityProbe probe,
    void *user_data)
{
    size_t index;
    if (!has_text(capability_id)) return UMI_STATUS_OK;
    for (index = 0U; index < status->entry_count; ++index) {
        if (strcmp(status->entries[index].capability_id, capability_id) == 0) {
            status->entries[index].consumer_count += 1U;
            return UMI_STATUS_OK;
        }
    }
    if (status->entry_count >= UMI_APPLICATION_RUNTIME_MAX_CAPABILITIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status->entries[status->entry_count].capability_id = capability_id;
    status->entries[status->entry_count].consumer_count = 1U;
    status->entries[status->entry_count].available =
        probe == NULL || probe(capability_id, user_data) != 0;
    if (!status->entries[status->entry_count].available) status->unavailable_count += 1U;
    status->entry_count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_capability_status_build(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationCapabilityStatus *out_status)
{
    size_t index;
    UmiStatus result;
    if (experience == NULL || out_status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_status, 0, sizeof(*out_status));
    out_status->structure_size = sizeof(*out_status);
    for (index = 0U; index < experience->panel_count; ++index) {
        result = add_capability(out_status, experience->panels[index].required_capability, probe, user_data);
        if (result != UMI_STATUS_OK) return result;
    }
    for (index = 0U; index < experience->feature_count; ++index) {
        result = add_capability(out_status, experience->features[index].required_capability, probe, user_data);
        if (result != UMI_STATUS_OK) return result;
    }
    return UMI_STATUS_OK;
}

const UmiApplicationCapabilityStatusEntry *umi_application_capability_status_find(
    const UmiApplicationCapabilityStatus *status,
    const char *capability_id)
{
    size_t index;
    if (status == NULL || capability_id == NULL) return NULL;
    for (index = 0U; index < status->entry_count; ++index) {
        if (strcmp(status->entries[index].capability_id, capability_id) == 0)
            return &status->entries[index];
    }
    return NULL;
}
