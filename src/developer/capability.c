/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capability.c
 *
 * PURPOSE:
 *   Validate and describe developer capability metadata without depending on
 *   Studio or a native UI toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capability.h"

#include <stdio.h>

/*
 * Provide the developer capability category text operation used by this module and its
 * client applications.
 */
const char *umi_developer_capability_category_text(
    UmiDeveloperCapabilityCategory category)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the developer capability maturity text operation used by this module and its
 * client applications.
 */
const char *umi_developer_capability_maturity_text(
    UmiDeveloperCapabilityMaturity maturity)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (maturity) {
        case UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED: return "planned";
        case UMI_DEVELOPER_CAPABILITY_MATURITY_EXPERIMENTAL: return "experimental";
        case UMI_DEVELOPER_CAPABILITY_MATURITY_STABLE: return "stable";
        default: return "unknown";
    }
}

/*
 * Check that developer capability descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_developer_capability_descriptor_validate(
    const UmiDeveloperCapabilityDescriptor *descriptor,
    char *out_message,
    size_t message_capacity)
{
    UmiStatus status = UMI_STATUS_OK;
    const char *message = "Developer capability descriptor is valid";
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->struct_size < sizeof(*descriptor)) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Developer capability structure is too small";
    } else /* Apply this branch only when its contract condition is satisfied. */ if (descriptor->api_version != UMI_DEVELOPER_CAPABILITY_API_VERSION) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Developer capability API version is unsupported";
    } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (descriptor->capability_id == NULL ||
               descriptor->capability_id[0] == '\0' ||
               descriptor->title == NULL || descriptor->title[0] == '\0' ||
               descriptor->summary == NULL || descriptor->summary[0] == '\0' ||
               descriptor->provider_role == NULL ||
               descriptor->provider_role[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Developer capability identity or descriptive text is empty";
    } else /* Apply this branch only when its contract condition is satisfied. */ if (descriptor->category <
                   UMI_DEVELOPER_CAPABILITY_CATEGORY_CONFIGURATION ||
               descriptor->category >
                   UMI_DEVELOPER_CAPABILITY_CATEGORY_PORTABILITY) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Developer capability category is invalid";
    } else /* Apply this branch only when its contract condition is satisfied. */ if (descriptor->maturity <
                   UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED ||
               descriptor->maturity >
                   UMI_DEVELOPER_CAPABILITY_MATURITY_STABLE) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Developer capability maturity is invalid";
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s", message);
    }
    return status;
}
