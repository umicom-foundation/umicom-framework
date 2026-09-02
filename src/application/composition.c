/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/composition.c
 *
 * PURPOSE:
 *   Resolve application definitions, presets, feature packs and component
 *   domains into one bounded Framework-owned composition plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/composition.h"

#include <string.h>

#include "umicom/application/component_catalogue.h"
#include "umicom/application/feature_pack.h"

/* Provide the copy id operation used by this module and its client applications. */
static UmiStatus copy_id(char *destination,
                         size_t capacity,
                         const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U ||
        source == NULL || source[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the contains id operation used by this module and its client applications. */
static int contains_id(
    const char items[][UMI_APPLICATION_COMPOSITION_ID_CAPACITY],
    size_t count,
    const char *id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id == NULL) return 0;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(items[index], id) == 0) return 1;
    }

    return 0;
}

/* Provide the add id operation used by this module and its client applications. */
static UmiStatus add_id(
    char items[][UMI_APPLICATION_COMPOSITION_ID_CAPACITY],
    size_t *count,
    size_t maximum,
    const char *id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL || count == NULL || id == NULL || id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (contains_id(items, *count, id)) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*count >= maximum) return UMI_STATUS_CAPACITY_EXCEEDED;

    status = copy_id(items[*count],
                     UMI_APPLICATION_COMPOSITION_ID_CAPACITY,
                     id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    *count += 1U;
    return UMI_STATUS_OK;
}

/* Provide the remove id operation used by this module and its client applications. */
static void remove_id(
    char items[][UMI_APPLICATION_COMPOSITION_ID_CAPACITY],
    size_t *count,
    const char *id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL || count == NULL || id == NULL) return;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < *count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(items[index], id) == 0) {
            const size_t tail = *count - index - 1U;

            /* Apply this branch only when its contract condition is satisfied. */
            if (tail > 0U) {
                (void)memmove(
                    &items[index],
                    &items[index + 1U],
                    tail * sizeof(items[0]));
            }

            *count -= 1U;
            (void)memset(&items[*count], 0, sizeof(items[0]));
            return;
        }
    }
}

/*
 * Provide the add required capability operation used by this module and its client
 * applications.
 */
static UmiStatus add_required_capability(
    UmiApplicationCompositionPlan *plan,
    const char *capability_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || capability_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    remove_id(plan->optional_capabilities,
              &plan->optional_capability_count,
              capability_id);

    return add_id(plan->required_capabilities,
                  &plan->required_capability_count,
                  UMI_APPLICATION_COMPOSITION_MAX_CAPABILITIES,
                  capability_id);
}

/*
 * Provide the add optional capability operation used by this module and its client
 * applications.
 */
static UmiStatus add_optional_capability(
    UmiApplicationCompositionPlan *plan,
    const char *capability_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || capability_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_id(plan->required_capabilities,
                    plan->required_capability_count,
                    capability_id)) {
        return UMI_STATUS_OK;
    }

    return add_id(plan->optional_capabilities,
                  &plan->optional_capability_count,
                  UMI_APPLICATION_COMPOSITION_MAX_CAPABILITIES,
                  capability_id);
}

/* Provide the add component operation used by this module and its client applications. */
static UmiStatus add_component(
    UmiApplicationCompositionPlan *plan,
    const char *component_id)
{
    const UmiApplicationComponentDefinition *component;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || component_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    component = umi_application_component_catalogue_find(component_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (component == NULL) return UMI_STATUS_NOT_FOUND;

    status = add_id(plan->component_ids,
                    &plan->component_count,
                    UMI_APPLICATION_COMPOSITION_MAX_COMPONENTS,
                    component_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /*
     * A selected component cannot function without its declared Framework
     * capability, so component dependencies become required automatically.
     */
    return add_required_capability(plan, component->capability_id);
}

/* Provide the add pack operation used by this module and its client applications. */
static UmiStatus add_pack(
    UmiApplicationCompositionPlan *plan,
    const char *pack_id)
{
    const UmiApplicationFeaturePackDefinition *pack;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || pack_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_id(plan->pack_ids, plan->pack_count, pack_id)) {
        return UMI_STATUS_OK;
    }

    pack = umi_application_feature_pack_catalogue_find(pack_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pack == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_application_feature_pack_validate(pack);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = add_id(plan->pack_ids,
                    &plan->pack_count,
                    UMI_APPLICATION_COMPOSITION_MAX_PACKS,
                    pack_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pack->required_capability_count; ++index) {
        status = add_required_capability(
            plan, pack->required_capabilities[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pack->optional_capability_count; ++index) {
        status = add_optional_capability(
            plan, pack->optional_capabilities[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pack->component_count; ++index) {
        status = add_component(plan, pack->component_ids[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the add definition domains operation used by this module and its client
 * applications.
 */
static UmiStatus add_definition_domains(
    const UmiApplicationDefinition *definition,
    UmiApplicationCompositionPlan *plan)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_application_component_catalogue_count();
         ++index) {
        const UmiApplicationComponentDefinition *component =
            umi_application_component_catalogue_at(index);

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (component != NULL &&
            umi_application_definition_uses_domain(
                definition,
                component->domain_id)) {
            const UmiStatus status =
                add_component(plan, component->component_id);

            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the evaluate capability operation used by this module and its client
 * applications.
 */
static UmiStatus evaluate_capability(
    const char *capability_id,
    UmiApplicationCompositionPlan *plan,
    int required)
{
    const UmiFrameworkCapabilityDefinition *capability =
        umi_framework_capability_catalogue_find(capability_id);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (capability == NULL) return UMI_STATUS_NOT_FOUND;

    /* Select the behaviour associated with the requested command or state value. */
    switch (capability->maturity) {
        case UMI_CAPABILITY_IMPLEMENTED:
            plan->implemented_capability_count += 1U;
            break;
        case UMI_CAPABILITY_FOUNDATION:
            plan->foundation_capability_count += 1U;
            /* Apply this operation only while the related capability or state is available. */
            if (required) plan->ready = 0;
            break;
        case UMI_CAPABILITY_PLANNED:
            plan->planned_capability_count += 1U;
            /* Apply this operation only while the related capability or state is available. */
            if (required) plan->ready = 0;
            break;
        default:
            return UMI_STATUS_INVALID_STATE;
    }

    return UMI_STATUS_OK;
}

/*
 * Initialise application composition request from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_composition_request_init(
    UmiApplicationCompositionRequest *request,
    const UmiApplicationDefinition *definition)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL) return;

    (void)memset(request, 0, sizeof(*request));
    request->structure_size = (uint32_t)sizeof(*request);
    request->api_version = UMI_APPLICATION_COMPOSITION_API_VERSION;
    request->definition = definition;
}

/*
 * Provide the application composition build operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_composition_build(
    const UmiApplicationCompositionRequest *request,
    UmiApplicationCompositionPlan *out_plan)
{
    const UmiApplicationDefinition *definition;
    const UmiApplicationPresetDefinition *preset = NULL;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || out_plan == NULL ||
        request->structure_size != sizeof(*request) ||
        request->api_version != UMI_APPLICATION_COMPOSITION_API_VERSION ||
        request->definition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    definition = request->definition;
    status = umi_application_definition_validate(definition);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->structure_size = (uint32_t)sizeof(*out_plan);
    out_plan->api_version = UMI_APPLICATION_COMPOSITION_API_VERSION;
    out_plan->ready = 1;
    out_plan->revision = 1U;

    status = copy_id(out_plan->application_id,
                     sizeof(out_plan->application_id),
                     definition->application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < definition->required_capability_count;
         ++index) {
        status = add_required_capability(
            out_plan,
            definition->required_capabilities[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < definition->optional_capability_count;
         ++index) {
        status = add_optional_capability(
            out_plan,
            definition->optional_capabilities[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request->preset_id != NULL && request->preset_id[0] != '\0') {
        preset = umi_application_preset_catalogue_find(request->preset_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (preset == NULL) return UMI_STATUS_NOT_FOUND;

        status = umi_application_preset_validate(preset);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = copy_id(out_plan->preset_id,
                         sizeof(out_plan->preset_id),
                         preset->preset_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < preset->feature_pack_count; ++index) {
            status = add_pack(out_plan, preset->feature_pack_ids[index]);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->additional_pack_count > 0U &&
        request->additional_pack_ids == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request->additional_pack_count; ++index) {
        status = add_pack(out_plan, request->additional_pack_ids[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->include_definition_domains) {
        status = add_definition_domains(definition, out_plan);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < out_plan->required_capability_count;
         ++index) {
        status = evaluate_capability(
            out_plan->required_capabilities[index],
            out_plan,
            1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < out_plan->optional_capability_count;
         ++index) {
        status = evaluate_capability(
            out_plan->optional_capabilities[index],
            out_plan,
            0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the application composition has pack operation used by this module and its
 * client applications.
 */
int umi_application_composition_has_pack(
    const UmiApplicationCompositionPlan *plan,
    const char *pack_id)
{
    return plan != NULL &&
        contains_id(plan->pack_ids, plan->pack_count, pack_id);
}

/*
 * Provide the application composition has capability operation used by this module and its
 * client applications.
 */
int umi_application_composition_has_capability(
    const UmiApplicationCompositionPlan *plan,
    const char *capability_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || capability_id == NULL) return 0;

    return contains_id(plan->required_capabilities,
                       plan->required_capability_count,
                       capability_id) ||
           contains_id(plan->optional_capabilities,
                       plan->optional_capability_count,
                       capability_id);
}

/*
 * Provide the application composition has component operation used by this module and its
 * client applications.
 */
int umi_application_composition_has_component(
    const UmiApplicationCompositionPlan *plan,
    const char *component_id)
{
    return plan != NULL &&
        contains_id(plan->component_ids,
                    plan->component_count,
                    component_id);
}
