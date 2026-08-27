/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_er_model.c
 *
 * PURPOSE:
 *   Verify deterministic entity placement and foreign-key relations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/data/workbench/er_model.h"

int main(void)
{
    UmiDataSchemaSnapshot schema;
    UmiDataSchemaTable table;
    UmiDataSchemaForeignKey foreign_key;
    UmiDataErModel model;
    umi_data_schema_snapshot_init(&schema);
    assert(umi_data_schema_table_init(&table, "customers", "customers") ==
           UMI_STATUS_OK);
    assert(umi_data_schema_snapshot_add(&schema, &table) == UMI_STATUS_OK);
    assert(umi_data_schema_table_init(&table, "orders", "orders") ==
           UMI_STATUS_OK);
    assert(umi_data_schema_snapshot_add(&schema, &table) == UMI_STATUS_OK);
    assert(umi_data_er_model_build(&model, &schema) == UMI_STATUS_OK);
    assert(umi_data_schema_foreign_key_init(
               &foreign_key, "fk-orders-customers", "orders", "customer_id",
               "customers", "customer_id") == UMI_STATUS_OK);
    assert(umi_data_er_model_add_relation(&model, &foreign_key) ==
           UMI_STATUS_OK);
    assert(model.entity_count == 2U && model.relation_count == 1U);
    return 0;
}
