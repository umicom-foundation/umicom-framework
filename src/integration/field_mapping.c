/* Umicom Framework | Integration field mapping | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/field_mapping.h"
#include <ctype.h>
#include <string.h>

UmiStatus umi_integration_field_mapping_init(UmiIntegrationFieldMapping *mapping,const char *id,const char *source_schema_id,const char *target_schema_id)
{
    UmiStatus status;
    if (mapping == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(mapping,0,sizeof(*mapping));
    status = umi_integration_designer_copy(mapping->id,sizeof(mapping->id),id);
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(mapping->source_schema_id,sizeof(mapping->source_schema_id),source_schema_id);
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(mapping->target_schema_id,sizeof(mapping->target_schema_id),target_schema_id);
    return status;
}

UmiStatus umi_integration_field_mapping_add(UmiIntegrationFieldMapping *mapping,const UmiIntegrationFieldRule *rule)
{
    if (mapping == NULL || rule == NULL || rule->target[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (rule->transform != UMI_INTEGRATION_DESIGNER_CONSTANT && rule->source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (mapping->rule_count >= UMI_INTEGRATION_DESIGNER_MAX_RULES) return UMI_STATUS_CAPACITY_EXCEEDED;
    mapping->rules[mapping->rule_count] = *rule;
    mapping->rule_count += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus transform_value(const UmiIntegrationFieldRule *rule,const char *value,char *output,size_t capacity)
{
    size_t index;
    UmiStatus status = umi_integration_designer_copy(output,capacity,rule->transform == UMI_INTEGRATION_DESIGNER_CONSTANT ? rule->constant_value : value);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; output[index] != '\0'; ++index) {
        unsigned char character = (unsigned char)output[index];
        if (rule->transform == UMI_INTEGRATION_DESIGNER_UPPERCASE) output[index] = (char)toupper(character);
        if (rule->transform == UMI_INTEGRATION_DESIGNER_LOWERCASE) output[index] = (char)tolower(character);
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_integration_field_mapping_apply(const UmiIntegrationFieldMapping *mapping,const UmiIntegrationDesignerRecord *source,UmiIntegrationDesignerRecord *target,UmiIntegrationDesignerValidation *validation)
{
    size_t index;
    char transformed[UMI_INTEGRATION_DESIGNER_TEXT_CAPACITY];
    if (mapping == NULL || source == NULL || target == NULL || validation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(target,0,sizeof(*target));
    validation->count = 0U;
    for (index = 0U; index < mapping->rule_count; ++index) {
        const UmiIntegrationFieldRule *rule = &mapping->rules[index];
        const char *value = rule->transform == UMI_INTEGRATION_DESIGNER_CONSTANT ? rule->constant_value : umi_integration_designer_record_get(source,rule->source);
        UmiStatus status;
        if (value == NULL) {
            if (rule->required) (void)umi_integration_designer_validation_add(validation,rule->source,"Required source field is missing");
            continue;
        }
        status = transform_value(rule,value,transformed,sizeof(transformed));
        if (status != UMI_STATUS_OK) return status;
        status = umi_integration_designer_record_set(target,rule->target,transformed);
        if (status != UMI_STATUS_OK) return status;
    }
    return validation->count == 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
