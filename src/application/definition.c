/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/definition.c
 *
 * PURPOSE:
 *   Validate application identity, capability and composition declarations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/definition.h"

#include <ctype.h>
#include <string.h>

/* Provide the valid identifier operation used by this module and its client applications. */
static int valid_identifier(const char *text)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || text[0] == '\0' ||
        !islower((unsigned char)text[0])) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; text[index] != '\0'; ++index) {
        const unsigned char value = (unsigned char)text[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!islower(value) && !isdigit(value) && value != '.' &&
            value != '-') return 0;
    }
    return 1;
}

/*
 * Provide the valid string array operation used by this module and its client
 * applications.
 */
static int valid_string_array(const char *const *items, size_t count)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (count > 0U && items == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (items[index] == NULL || items[index][0] == '\0') return 0;
    }
    return 1;
}

/*
 * Check that application definition satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_application_definition_validate(
    const UmiApplicationDefinition *definition)
{
    const uint32_t required_flags = UMI_APPLICATION_STANDALONE |
                                    UMI_APPLICATION_FEDERATED;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL ||
        definition->structure_size < sizeof(*definition) ||
        !valid_identifier(definition->application_id) ||
        definition->display_name == NULL ||
        definition->display_name[0] == '\0' ||
        definition->repository_slug == NULL ||
        definition->repository_slug[0] == '\0' ||
        definition->executable_name == NULL ||
        definition->executable_name[0] == '\0' ||
        definition->purpose == NULL || definition->purpose[0] == '\0' ||
        definition->family < UMI_APPLICATION_FAMILY_PLATFORM ||
        definition->family > UMI_APPLICATION_FAMILY_EDUCATION ||
        definition->maturity < UMI_APPLICATION_AVAILABLE ||
        definition->maturity > UMI_APPLICATION_ROADMAP ||
        definition->frontends == UMI_FRONTEND_NONE ||
        (definition->flags & required_flags) != required_flags ||
        definition->required_capability_count == 0U ||
        definition->component_domain_count == 0U ||
        definition->workspace_profile_count == 0U ||
        !valid_string_array(definition->required_capabilities,
                            definition->required_capability_count) ||
        !valid_string_array(definition->optional_capabilities,
                            definition->optional_capability_count) ||
        !valid_string_array(definition->component_domains,
                            definition->component_domain_count) ||
        !valid_string_array(definition->workspace_profiles,
                            definition->workspace_profile_count)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the array contains operation used by this module and its client applications. */
static int array_contains(const char *const *items, size_t count,
                          const char *value)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL || value == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(items[index], value) == 0) return 1;
    }
    return 0;
}

/*
 * Provide the application definition declares capability operation used by this module and
 * its client applications.
 */
int umi_application_definition_declares_capability(
    const UmiApplicationDefinition *definition,
    const char *capability_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL || capability_id == NULL) return 0;
    return array_contains(definition->required_capabilities,
                          definition->required_capability_count,
                          capability_id) ||
           array_contains(definition->optional_capabilities,
                          definition->optional_capability_count,
                          capability_id);
}

/*
 * Provide the application definition uses domain operation used by this module and its
 * client applications.
 */
int umi_application_definition_uses_domain(
    const UmiApplicationDefinition *definition,
    const char *domain_id)
{
    return definition != NULL && domain_id != NULL &&
        array_contains(definition->component_domains,
                       definition->component_domain_count, domain_id);
}

/*
 * Provide the application family text operation used by this module and its client
 * applications.
 */
const char *umi_application_family_text(UmiApplicationFamily family)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (family) {
        case UMI_APPLICATION_FAMILY_PLATFORM: return "platform";
        case UMI_APPLICATION_FAMILY_DEVELOPMENT: return "development";
        case UMI_APPLICATION_FAMILY_FINANCE: return "finance";
        case UMI_APPLICATION_FAMILY_MEDIA: return "media";
        case UMI_APPLICATION_FAMILY_AI: return "ai";
        case UMI_APPLICATION_FAMILY_DESIGN: return "design";
        case UMI_APPLICATION_FAMILY_GAMES: return "games";
        case UMI_APPLICATION_FAMILY_ENTERPRISE: return "enterprise";
        case UMI_APPLICATION_FAMILY_OPERATING_SYSTEM:
            return "operating-system";
        case UMI_APPLICATION_FAMILY_EDUCATION: return "education";
        default: return "unknown";
    }
}

/*
 * Provide the application maturity text operation used by this module and its client
 * applications.
 */
const char *umi_application_maturity_text(UmiApplicationMaturity maturity)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (maturity) {
        case UMI_APPLICATION_AVAILABLE: return "available";
        case UMI_APPLICATION_FOUNDATION: return "foundation";
        case UMI_APPLICATION_ROADMAP: return "roadmap";
        default: return "unknown";
    }
}
