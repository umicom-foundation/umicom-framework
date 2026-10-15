/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_field_mapping.c
 *
 * PURPOSE:
 *   Implement the test integration field mapping behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Field mapping | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "value_archive/construction_cases.h"
#include <string.h>
#include "umicom/integration/field_mapping.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted field mapping value. */
static int UmiIntegrationFieldMappingConstructionCases(void)
{
    UmiIntegrationFieldMapping *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_integration_field_mapping_init(value, "order-map", "source", "target") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_integration_field_mapping_init(value, oversized, "source", "target") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_integration_field_mapping_init(value, value->id, "source", "target") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->id, "order-map") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->source_schema_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_integration_field_mapping_init(value, "order-map", oversized, "target") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_integration_field_mapping_init(value, "order-map", value->source_schema_id, "target") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->source_schema_id, "source") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->target_schema_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_integration_field_mapping_init(value, "order-map", "source", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_integration_field_mapping_init(value, "order-map", "source", value->target_schema_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->target_schema_id, "target") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiIntegrationFieldMappingConstructionCases() != 0) return 1;

    UmiIntegrationFieldMapping mapping;
    UmiIntegrationFieldRule copy = {0};
    UmiIntegrationFieldRule constant = {0};
    UmiIntegrationDesignerRecord source = {0};
    UmiIntegrationDesignerRecord target = {0};
    UmiIntegrationDesignerValidation validation = {0};
    assert(umi_integration_field_mapping_init(&mapping,"order-map","source","target") == UMI_STATUS_OK);
    (void)umi_integration_designer_copy(copy.source,sizeof(copy.source),"desk"); (void)umi_integration_designer_copy(copy.target,sizeof(copy.target),"BOOK"); copy.transform = UMI_INTEGRATION_DESIGNER_UPPERCASE; copy.required = true;
    (void)umi_integration_designer_copy(constant.target,sizeof(constant.target),"source"); (void)umi_integration_designer_copy(constant.constant_value,sizeof(constant.constant_value),"umicom"); constant.transform = UMI_INTEGRATION_DESIGNER_CONSTANT;
    assert(umi_integration_field_mapping_add(&mapping,&copy) == UMI_STATUS_OK);
    assert(umi_integration_field_mapping_add(&mapping,&constant) == UMI_STATUS_OK);
    assert(umi_integration_designer_record_set(&source,"desk","treasury") == UMI_STATUS_OK);
    assert(umi_integration_field_mapping_apply(&mapping,&source,&target,&validation) == UMI_STATUS_OK);
    assert(strcmp(umi_integration_designer_record_get(&target,"BOOK"),"TREASURY") == 0);
    return 0;
}
