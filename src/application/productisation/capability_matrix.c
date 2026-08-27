/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/capability_matrix.c
 *
 * PURPOSE:
 *   Derive one reusable-capability usage matrix from all canonical application
 *   features, panels and Framework component definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/capability_matrix.h"

#include <string.h>

#include "umicom/application/component_catalogue.h"
#include "umicom/application/experience_catalogue.h"

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static UmiProductCapabilityUsage *find_mutable(
    UmiProductCapabilityMatrix *matrix, const char *capability_id)
{
    size_t index;
    for (index = 0U; index < matrix->usage_count; ++index) {
        if (strcmp(matrix->usages[index].capability_id, capability_id) == 0)
            return &matrix->usages[index];
    }
    return NULL;
}

static UmiStatus touch_usage(UmiProductCapabilityMatrix *matrix,
                             const char *capability_id,
                             size_t application_index,
                             int feature_reference,
                             int panel_reference)
{
    UmiProductCapabilityUsage *usage;
    const UmiFrameworkCapabilityDefinition *capability;
    const uint64_t application_bit = UINT64_C(1) << application_index;
    UmiStatus status;

    if (capability_id == NULL || capability_id[0] == '\0')
        return UMI_STATUS_OK;
    usage = find_mutable(matrix, capability_id);
    if (usage == NULL) {
        if (matrix->usage_count >= UMI_PRODUCTISATION_MAX_CAPABILITIES)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        usage = &matrix->usages[matrix->usage_count++];
        (void)memset(usage, 0, sizeof(*usage));
        status = copy_text(usage->capability_id,
                           sizeof(usage->capability_id), capability_id);
        if (status != UMI_STATUS_OK) return status;
        capability = umi_framework_capability_catalogue_find(capability_id);
        if (capability == NULL) {
            usage->missing_from_framework = 1;
            status = copy_text(usage->category, sizeof(usage->category),
                               "unregistered");
        } else {
            usage->maturity = capability->maturity;
            status = copy_text(usage->category, sizeof(usage->category),
                               capability->category);
        }
        if (status != UMI_STATUS_OK) return status;
    }
    if ((usage->application_mask & application_bit) == 0U) {
        usage->application_mask |= application_bit;
        usage->application_count += 1U;
    }
    if (feature_reference) usage->feature_reference_count += 1U;
    if (panel_reference) usage->panel_reference_count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_product_capability_matrix_build(
    UmiProductCapabilityMatrix *out_matrix)
{
    size_t application_index;
    size_t usage_index;
    size_t component_index;
    const size_t application_count =
        umi_application_experience_catalogue_count();

    if (out_matrix == NULL ||
        application_count > UMI_PRODUCTISATION_MAX_APPLICATIONS)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_matrix, 0, sizeof(*out_matrix));

    for (application_index = 0U; application_index < application_count;
         ++application_index) {
        const UmiApplicationExperienceDefinition *definition =
            umi_application_experience_catalogue_at(application_index);
        size_t index;
        if (definition == NULL) return UMI_STATUS_INVALID_STATE;
        for (index = 0U; index < definition->feature_count; ++index) {
            UmiStatus status = touch_usage(out_matrix,
                definition->features[index].required_capability,
                application_index, 1, 0);
            if (status != UMI_STATUS_OK) return status;
        }
        for (index = 0U; index < definition->panel_count; ++index) {
            UmiStatus status = touch_usage(out_matrix,
                definition->panels[index].required_capability,
                application_index, 0, 1);
            if (status != UMI_STATUS_OK) return status;
        }
    }

    for (component_index = 0U;
         component_index < umi_application_component_catalogue_count();
         ++component_index) {
        const UmiApplicationComponentDefinition *component =
            umi_application_component_catalogue_at(component_index);
        UmiProductCapabilityUsage *usage;
        if (component == NULL) continue;
        usage = find_mutable(out_matrix, component->capability_id);
        if (usage != NULL) usage->component_count += 1U;
    }

    for (usage_index = 0U; usage_index < out_matrix->usage_count;
         ++usage_index) {
        UmiProductCapabilityUsage *usage = &out_matrix->usages[usage_index];
        usage->shared_across_applications = usage->application_count >= 2U;
        if (usage->shared_across_applications) out_matrix->shared_count += 1U;
        if (usage->missing_from_framework) out_matrix->missing_count += 1U;
        else if (usage->maturity == UMI_CAPABILITY_IMPLEMENTED)
            out_matrix->implemented_count += 1U;
        else if (usage->maturity == UMI_CAPABILITY_FOUNDATION)
            out_matrix->foundation_count += 1U;
        else if (usage->maturity == UMI_CAPABILITY_PLANNED)
            out_matrix->planned_count += 1U;
        if (usage->panel_reference_count > 0U && usage->component_count == 0U)
            out_matrix->missing_component_count += 1U;
    }
    return UMI_STATUS_OK;
}

const UmiProductCapabilityUsage *umi_product_capability_matrix_find(
    const UmiProductCapabilityMatrix *matrix,
    const char *capability_id)
{
    size_t index;
    if (matrix == NULL || capability_id == NULL) return NULL;
    for (index = 0U; index < matrix->usage_count; ++index) {
        if (strcmp(matrix->usages[index].capability_id, capability_id) == 0)
            return &matrix->usages[index];
    }
    return NULL;
}

int umi_product_capability_usage_has_application(
    const UmiProductCapabilityUsage *usage,
    size_t application_index)
{
    if (usage == NULL || application_index >= 64U) return 0;
    return (usage->application_mask &
            (UINT64_C(1) << application_index)) != 0U;
}
