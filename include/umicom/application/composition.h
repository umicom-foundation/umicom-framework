/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/composition.h
 *
 * PURPOSE:
 *   Resolve a thin application definition plus reusable feature packs into one
 *   deterministic list of Framework capabilities and component surfaces.
 *
 * ARCHITECTURE:
 *   Applications describe intent. Framework owns capability identities,
 *   reusable components, maturity evidence and composition rules. The resulting
 *   plan can drive Studio New Project, native scaffolding, installers, Desk and
 *   future application generators without copying feature inventories.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPOSITION_H
#define UMICOM_APPLICATION_COMPOSITION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/application/definition.h"
#include "umicom/application/preset.h"
#include "umicom/runtime/capability_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_COMPOSITION_API_VERSION 1U
#define UMI_APPLICATION_COMPOSITION_ID_CAPACITY 192U
#define UMI_APPLICATION_COMPOSITION_MAX_PACKS 24U
#define UMI_APPLICATION_COMPOSITION_MAX_CAPABILITIES 160U
#define UMI_APPLICATION_COMPOSITION_MAX_COMPONENTS 192U

typedef struct UmiApplicationCompositionRequest {
    uint32_t structure_size;
    uint32_t api_version;
    const UmiApplicationDefinition *definition;
    const char *preset_id;
    const char *const *additional_pack_ids;
    size_t additional_pack_count;

    /*
     * Include every canonical Framework component whose domain appears in the
     * application definition. This is useful for generated workbenches and the
     * Studio New Project preview.
     */
    int include_definition_domains;
} UmiApplicationCompositionRequest;

typedef struct UmiApplicationCompositionPlan {
    uint32_t structure_size;
    uint32_t api_version;
    char application_id[UMI_APPLICATION_COMPOSITION_ID_CAPACITY];
    char preset_id[UMI_APPLICATION_COMPOSITION_ID_CAPACITY];

    char pack_ids[UMI_APPLICATION_COMPOSITION_MAX_PACKS]
                 [UMI_APPLICATION_COMPOSITION_ID_CAPACITY];
    size_t pack_count;

    char required_capabilities[UMI_APPLICATION_COMPOSITION_MAX_CAPABILITIES]
                              [UMI_APPLICATION_COMPOSITION_ID_CAPACITY];
    size_t required_capability_count;

    char optional_capabilities[UMI_APPLICATION_COMPOSITION_MAX_CAPABILITIES]
                              [UMI_APPLICATION_COMPOSITION_ID_CAPACITY];
    size_t optional_capability_count;

    char component_ids[UMI_APPLICATION_COMPOSITION_MAX_COMPONENTS]
                      [UMI_APPLICATION_COMPOSITION_ID_CAPACITY];
    size_t component_count;

    size_t implemented_capability_count;
    size_t foundation_capability_count;
    size_t planned_capability_count;

    /*
     * ready means every REQUIRED capability is currently IMPLEMENTED.
     * Optional foundation/roadmap capabilities remain visible without blocking
     * a runnable thin application.
     */
    int ready;
    uint64_t revision;
} UmiApplicationCompositionPlan;

void umi_application_composition_request_init(
    UmiApplicationCompositionRequest *request,
    const UmiApplicationDefinition *definition);

UmiStatus umi_application_composition_build(
    const UmiApplicationCompositionRequest *request,
    UmiApplicationCompositionPlan *out_plan);

int umi_application_composition_has_pack(
    const UmiApplicationCompositionPlan *plan,
    const char *pack_id);

int umi_application_composition_has_capability(
    const UmiApplicationCompositionPlan *plan,
    const char *capability_id);

int umi_application_composition_has_component(
    const UmiApplicationCompositionPlan *plan,
    const char *component_id);

#ifdef __cplusplus
}
#endif

#endif
