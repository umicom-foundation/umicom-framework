/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component_catalogue.h
 *
 * PURPOSE:
 *   Catalogue reusable cross-application surfaces by domain. These definitions
 *   belong to Framework; applications select and arrange them without copying
 *   their models, commands, policies or rendering contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_CATALOGUE_H
#define UMICOM_APPLICATION_COMPONENT_CATALOGUE_H

#include <stddef.h>

#include "umicom/runtime/capability_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiApplicationComponentRole {
    UMI_APPLICATION_COMPONENT_VIEW = 1,
    UMI_APPLICATION_COMPONENT_EDITOR = 2,
    UMI_APPLICATION_COMPONENT_TOOL = 3,
    UMI_APPLICATION_COMPONENT_DASHBOARD = 4,
    UMI_APPLICATION_COMPONENT_INSPECTOR = 5,
    UMI_APPLICATION_COMPONENT_SERVICE_SURFACE = 6
} UmiApplicationComponentRole;

typedef struct UmiApplicationComponentDefinition {
    const char *component_id;
    const char *domain_id;
    const char *title;
    const char *capability_id;
    const char *description;
    UmiApplicationComponentRole role;
    UmiCapabilityMaturity maturity;
    int multi_instance;
    int context_aware;
} UmiApplicationComponentDefinition;

size_t umi_application_component_catalogue_count(void);
const UmiApplicationComponentDefinition *
umi_application_component_catalogue_at(size_t index);
const UmiApplicationComponentDefinition *
umi_application_component_catalogue_find(const char *component_id);
size_t umi_application_component_domain_count(const char *domain_id);
int umi_application_component_domain_exists(const char *domain_id);
const char *umi_application_component_role_text(
    UmiApplicationComponentRole role);

#ifdef __cplusplus
}
#endif

#endif
