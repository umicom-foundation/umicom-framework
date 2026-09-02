/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/schema_column.c
 *
 * PURPOSE:
 *   Describe portable column metadata used by schema diffing, ORM mapping and migrations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_column.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_schema_column_init(UmiDataSchemaColumn *item, const char *column_id, const char *name, UmiDataValueKind kind, uint32_t ordinal, bool nullable) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->column_id,sizeof(item->column_id),column_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_data_enterprise_copy_text(item->name,sizeof(item->name),name); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    item->kind=kind; item->ordinal=ordinal; item->nullable=nullable; item->generated=false;
    return umi_data_schema_column_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_schema_column_validate(const UmiDataSchemaColumn *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(item->column_id[0] != '\0' && item->name[0] != '\0' && item->kind >= UMI_DATA_VALUE_INTEGER && item->kind <= UMI_DATA_VALUE_DECIMAL)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
