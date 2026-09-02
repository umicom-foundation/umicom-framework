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

/**
 * Represent the application experience registry data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationExperienceRegistry {
    const UmiApplicationExperienceDefinition *
        definitions[UMI_APPLICATION_EXPERIENCE_REGISTRY_CAPACITY];
    size_t count;
} UmiApplicationExperienceRegistry;

/**
 * Initialise application experience registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_experience_registry_init(
    UmiApplicationExperienceRegistry *registry);

/**
 * Add application experience registry only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_application_experience_registry_register(
    UmiApplicationExperienceRegistry *registry,
    const UmiApplicationExperienceDefinition *definition);

/**
 * Return the number of records represented by application experience registry without
 * changing their state.
 */
size_t umi_application_experience_registry_count(
    const UmiApplicationExperienceRegistry *registry);

/**
 * Find application experience registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationExperienceDefinition *
umi_application_experience_registry_at(
    const UmiApplicationExperienceRegistry *registry,
    size_t index);

/**
 * Find application experience registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationExperienceDefinition *
umi_application_experience_registry_find(
    const UmiApplicationExperienceRegistry *registry,
    const char *application_id);

#ifdef __cplusplus
}
#endif

#endif
