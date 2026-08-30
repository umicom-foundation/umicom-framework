/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/er_model.h
 *
 * PURPOSE:
 *   Project canonical schema tables and foreign keys into a deterministic,
 *   renderer-neutral entity-relationship diagram.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_ER_MODEL_H
#define UMICOM_DATA_WORKBENCH_ER_MODEL_H

#include "umicom/data/enterprise/schema_foreign_key.h"
#include "umicom/data/enterprise/schema_snapshot.h"
#include "umicom/data/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataErEntity {
    char table_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char label[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    size_t column_count;
    int32_t x;
    int32_t y;
    int selected;
} UmiDataErEntity;

typedef struct UmiDataErRelation {
    UmiDataSchemaForeignKey foreign_key;
    size_t source_entity;
    size_t target_entity;
} UmiDataErRelation;

typedef struct UmiDataErModel {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDataErEntity entities[UMI_DATA_ENTERPRISE_MAX_ITEMS];
    UmiDataErRelation relations[UMI_DATA_WORKBENCH_MAX_RELATIONS];
    size_t entity_count;
    size_t relation_count;
    size_t selected_entity;
    int truncated;
    uint64_t revision;
} UmiDataErModel;

void umi_data_er_model_init(UmiDataErModel *model);
UmiStatus umi_data_er_model_build(
    UmiDataErModel *model,
    const UmiDataSchemaSnapshot *schema);
UmiStatus umi_data_er_model_add_relation(
    UmiDataErModel *model,
    const UmiDataSchemaForeignKey *foreign_key);
UmiStatus umi_data_er_model_select(
    UmiDataErModel *model,
    size_t entity_index);
const UmiDataErEntity *umi_data_er_model_entity_at(
    const UmiDataErModel *model,
    size_t entity_index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_ER_MODEL_H */
