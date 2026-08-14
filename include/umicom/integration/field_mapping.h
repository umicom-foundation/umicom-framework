/* Umicom Framework | Integration field mapping | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_INTEGRATION_FIELD_MAPPING_H
#define UMICOM_INTEGRATION_FIELD_MAPPING_H
#include "umicom/integration/designer_types.h"
typedef struct UmiIntegrationFieldRule {
    char source[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char target[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    UmiIntegrationDesignerTransform transform;
    char constant_value[UMI_INTEGRATION_DESIGNER_TEXT_CAPACITY];
    bool required;
} UmiIntegrationFieldRule;
typedef struct UmiIntegrationFieldMapping {
    char id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char source_schema_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char target_schema_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    UmiIntegrationFieldRule rules[UMI_INTEGRATION_DESIGNER_MAX_RULES];
    size_t rule_count;
} UmiIntegrationFieldMapping;
UmiStatus umi_integration_field_mapping_init(UmiIntegrationFieldMapping *mapping,const char *id,const char *source_schema_id,const char *target_schema_id);
UmiStatus umi_integration_field_mapping_add(UmiIntegrationFieldMapping *mapping,const UmiIntegrationFieldRule *rule);
UmiStatus umi_integration_field_mapping_apply(const UmiIntegrationFieldMapping *mapping,const UmiIntegrationDesignerRecord *source,UmiIntegrationDesignerRecord *target,UmiIntegrationDesignerValidation *validation);
#endif
