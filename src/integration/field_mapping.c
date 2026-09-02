/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/field_mapping.c
 *
 * PURPOSE:
 *   Implement the field mapping behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Integration field mapping | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/field_mapping.h"
#include <ctype.h>
#include <string.h>

/*
 * Initialise integration field mapping from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_integration_field_mapping_init(UmiIntegrationFieldMapping *mapping,const char *id,const char *source_schema_id,const char *target_schema_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mapping == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(mapping,0,sizeof(*mapping));
    status = umi_integration_designer_copy(mapping->id,sizeof(mapping->id),id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(mapping->source_schema_id,sizeof(mapping->source_schema_id),source_schema_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(mapping->target_schema_id,sizeof(mapping->target_schema_id),target_schema_id);
    return status;
}

/*
 * Add integration field mapping only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_integration_field_mapping_add(UmiIntegrationFieldMapping *mapping,const UmiIntegrationFieldRule *rule)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mapping == NULL || rule == NULL || rule->target[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (rule->transform != UMI_INTEGRATION_DESIGNER_CONSTANT && rule->source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (mapping->rule_count >= UMI_INTEGRATION_DESIGNER_MAX_RULES) return UMI_STATUS_CAPACITY_EXCEEDED;
    mapping->rules[mapping->rule_count] = *rule;
    mapping->rule_count += 1U;
    return UMI_STATUS_OK;
}

/* Provide the transform value operation used by this module and its client applications. */
static UmiStatus transform_value(const UmiIntegrationFieldRule *rule,const char *value,char *output,size_t capacity)
{
    size_t index;
    UmiStatus status = umi_integration_designer_copy(output,capacity,rule->transform == UMI_INTEGRATION_DESIGNER_CONSTANT ? rule->constant_value : value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; output[index] != '\0'; ++index) {
        unsigned char character = (unsigned char)output[index];
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (rule->transform == UMI_INTEGRATION_DESIGNER_UPPERCASE) output[index] = (char)toupper(character);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (rule->transform == UMI_INTEGRATION_DESIGNER_LOWERCASE) output[index] = (char)tolower(character);
    }
    return UMI_STATUS_OK;
}

/*
 * Perform integration field mapping through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_integration_field_mapping_apply(const UmiIntegrationFieldMapping *mapping,const UmiIntegrationDesignerRecord *source,UmiIntegrationDesignerRecord *target,UmiIntegrationDesignerValidation *validation)
{
    size_t index;
    char transformed[UMI_INTEGRATION_DESIGNER_TEXT_CAPACITY];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mapping == NULL || source == NULL || target == NULL || validation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(target,0,sizeof(*target));
    validation->count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < mapping->rule_count; ++index) {
        const UmiIntegrationFieldRule *rule = &mapping->rules[index];
        const char *value = rule->transform == UMI_INTEGRATION_DESIGNER_CONSTANT ? rule->constant_value : umi_integration_designer_record_get(source,rule->source);
        UmiStatus status;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (value == NULL) {
            /* Apply this operation only while the related capability or state is available. */
            if (rule->required) (void)umi_integration_designer_validation_add(validation,rule->source,"Required source field is missing");
            continue;
        }
        status = transform_value(rule,value,transformed,sizeof(transformed));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_integration_designer_record_set(target,rule->target,transformed);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return validation->count == 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
