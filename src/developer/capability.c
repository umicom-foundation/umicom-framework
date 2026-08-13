/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capability.c
 *
 * PURPOSE:
 *   Validate and describe developer capability metadata without depending on
 *   Studio or a native UI toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capability.h"

#include <stdio.h>

const char *umi_developer_capability_category_text(
    UmiDeveloperCapabilityCategory category)
{
    switch (category) {
        case UMI_DEVELOPER_CAPABILITY_CATEGORY_CONFIGURATION: return "configuration";
        case UMI_DEVELOPER_CAPABILITY_CATEGORY_BUILD: return "build";
        case UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING: return "testing";
        case UMI_DEVELOPER_CAPABILITY_CATEGORY_EXECUTION: return "execution";
        case UMI_DEVELOPER_CAPABILITY_CATEGORY_TERMINAL: return "terminal";
        case UMI_DEVELOPER_CAPABILITY_CATEGORY_DELIVERY: return "delivery";
        case UMI_DEVELOPER_CAPABILITY_CATEGORY_DIAGNOSTICS: return "diagnostics";
        case UMI_DEVELOPER_CAPABILITY_CATEGORY_SECURITY: return "security";
        case UMI_DEVELOPER_CAPABILITY_CATEGORY_PERFORMANCE: return "performance";
        case UMI_DEVELOPER_CAPABILITY_CATEGORY_INTEGRATION: return "integration";
        case UMI_DEVELOPER_CAPABILITY_CATEGORY_EXPERIENCE: return "experience";
        case UMI_DEVELOPER_CAPABILITY_CATEGORY_PORTABILITY: return "portability";
        default: return "unknown";
    }
}

const char *umi_developer_capability_maturity_text(
    UmiDeveloperCapabilityMaturity maturity)
{
    switch (maturity) {
        case UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED: return "planned";
        case UMI_DEVELOPER_CAPABILITY_MATURITY_EXPERIMENTAL: return "experimental";
        case UMI_DEVELOPER_CAPABILITY_MATURITY_STABLE: return "stable";
        default: return "unknown";
    }
}

UmiStatus umi_developer_capability_descriptor_validate(
    const UmiDeveloperCapabilityDescriptor *descriptor,
    char *out_message,
    size_t message_capacity)
{
    UmiStatus status = UMI_STATUS_OK;
    const char *message = "Developer capability descriptor is valid";
    if (descriptor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (descriptor->struct_size < sizeof(*descriptor)) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Developer capability structure is too small";
    } else if (descriptor->api_version != UMI_DEVELOPER_CAPABILITY_API_VERSION) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Developer capability API version is unsupported";
    } else if (descriptor->capability_id == NULL ||
               descriptor->capability_id[0] == '\0' ||
               descriptor->title == NULL || descriptor->title[0] == '\0' ||
               descriptor->summary == NULL || descriptor->summary[0] == '\0' ||
               descriptor->provider_role == NULL ||
               descriptor->provider_role[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Developer capability identity or descriptive text is empty";
    } else if (descriptor->category <
                   UMI_DEVELOPER_CAPABILITY_CATEGORY_CONFIGURATION ||
               descriptor->category >
                   UMI_DEVELOPER_CAPABILITY_CATEGORY_PORTABILITY) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Developer capability category is invalid";
    } else if (descriptor->maturity <
                   UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED ||
               descriptor->maturity >
                   UMI_DEVELOPER_CAPABILITY_MATURITY_STABLE) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Developer capability maturity is invalid";
    }
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s", message);
    }
    return status;
}
