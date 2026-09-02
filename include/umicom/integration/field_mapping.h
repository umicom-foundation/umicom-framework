/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/field_mapping.h
 *
 * PURPOSE:
 *   Publish the public field mapping contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FIELD_MAPPING_H
#define UMICOM_INTEGRATION_FIELD_MAPPING_H
#include "umicom/integration/designer_types.h"
/**
 * Represent the integration field rule data shared with callers of this public contract.
 */
typedef struct UmiIntegrationFieldRule {
    char source[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char target[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    UmiIntegrationDesignerTransform transform;
    char constant_value[UMI_INTEGRATION_DESIGNER_TEXT_CAPACITY];
    bool required;
} UmiIntegrationFieldRule;
/**
 * Represent the integration field mapping data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationFieldMapping {
    char id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char source_schema_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char target_schema_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    UmiIntegrationFieldRule rules[UMI_INTEGRATION_DESIGNER_MAX_RULES];
    size_t rule_count;
} UmiIntegrationFieldMapping;
/**
 * Initialise integration field mapping from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_integration_field_mapping_init(UmiIntegrationFieldMapping *mapping,const char *id,const char *source_schema_id,const char *target_schema_id);
/**
 * Add integration field mapping only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_integration_field_mapping_add(UmiIntegrationFieldMapping *mapping,const UmiIntegrationFieldRule *rule);
/**
 * Perform integration field mapping through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_integration_field_mapping_apply(const UmiIntegrationFieldMapping *mapping,const UmiIntegrationDesignerRecord *source,UmiIntegrationDesignerRecord *target,UmiIntegrationDesignerValidation *validation);
#endif
