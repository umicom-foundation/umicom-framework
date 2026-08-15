/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/definition.h
 *
 * PURPOSE:
 *   Define the authoritative, toolkit-neutral contract for an independently
 *   runnable Umicom application. Applications declare what they consume; the
 *   Framework owns every reusable capability and communication mechanism.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_DEFINITION_H
#define UMICOM_APPLICATION_DEFINITION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/runtime/application_manifest.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiApplicationFamily {
    UMI_APPLICATION_FAMILY_PLATFORM = 1,
    UMI_APPLICATION_FAMILY_DEVELOPMENT = 2,
    UMI_APPLICATION_FAMILY_FINANCE = 3,
    UMI_APPLICATION_FAMILY_MEDIA = 4,
    UMI_APPLICATION_FAMILY_AI = 5,
    UMI_APPLICATION_FAMILY_DESIGN = 6,
    UMI_APPLICATION_FAMILY_GAMES = 7,
    UMI_APPLICATION_FAMILY_ENTERPRISE = 8,
    UMI_APPLICATION_FAMILY_OPERATING_SYSTEM = 9,
    UMI_APPLICATION_FAMILY_EDUCATION = 10
} UmiApplicationFamily;

typedef enum UmiApplicationMaturity {
    UMI_APPLICATION_AVAILABLE = 1,
    UMI_APPLICATION_FOUNDATION = 2,
    UMI_APPLICATION_ROADMAP = 3
} UmiApplicationMaturity;

typedef enum UmiApplicationFlags {
    UMI_APPLICATION_STANDALONE = 1U << 0,
    UMI_APPLICATION_FEDERATED = 1U << 1,
    UMI_APPLICATION_MULTI_WINDOW = 1U << 2,
    UMI_APPLICATION_MULTI_MONITOR = 1U << 3,
    UMI_APPLICATION_HEADLESS_SAFE = 1U << 4,
    UMI_APPLICATION_SANDBOX_RECOMMENDED = 1U << 5
} UmiApplicationFlags;

typedef struct UmiApplicationDefinition {
    uint32_t structure_size;
    const char *application_id;
    const char *display_name;
    const char *repository_slug;
    const char *executable_name;
    const char *purpose;
    UmiApplicationFamily family;
    UmiApplicationMaturity maturity;
    unsigned frontends;
    uint32_t flags;
    const char *const *required_capabilities;
    size_t required_capability_count;
    const char *const *optional_capabilities;
    size_t optional_capability_count;
    const char *const *component_domains;
    size_t component_domain_count;
    const char *const *workspace_profiles;
    size_t workspace_profile_count;
} UmiApplicationDefinition;

UmiStatus umi_application_definition_validate(
    const UmiApplicationDefinition *definition);
int umi_application_definition_declares_capability(
    const UmiApplicationDefinition *definition,
    const char *capability_id);
int umi_application_definition_uses_domain(
    const UmiApplicationDefinition *definition,
    const char *domain_id);
const char *umi_application_family_text(UmiApplicationFamily family);
const char *umi_application_maturity_text(UmiApplicationMaturity maturity);

#ifdef __cplusplus
}
#endif

#endif
