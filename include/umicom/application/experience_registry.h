/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience_registry.h
 *
 * PURPOSE:
 *   Provide a bounded registry for Framework-owned application experience definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCE_REGISTRY_H
#define UMICOM_APPLICATION_EXPERIENCE_REGISTRY_H

#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationExperienceRegistry {
    const UmiApplicationExperienceDefinition *
        definitions[UMI_APPLICATION_EXPERIENCE_REGISTRY_CAPACITY];
    size_t count;
} UmiApplicationExperienceRegistry;

void umi_application_experience_registry_init(
    UmiApplicationExperienceRegistry *registry);

UmiStatus umi_application_experience_registry_register(
    UmiApplicationExperienceRegistry *registry,
    const UmiApplicationExperienceDefinition *definition);

size_t umi_application_experience_registry_count(
    const UmiApplicationExperienceRegistry *registry);

const UmiApplicationExperienceDefinition *
umi_application_experience_registry_at(
    const UmiApplicationExperienceRegistry *registry,
    size_t index);

const UmiApplicationExperienceDefinition *
umi_application_experience_registry_find(
    const UmiApplicationExperienceRegistry *registry,
    const char *application_id);

#ifdef __cplusplus
}
#endif

#endif
