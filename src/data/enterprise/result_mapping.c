/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/result_mapping.c
 *
 * PURPOSE:
 *   Describe how one result column maps back into an ORM field.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/result_mapping.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_result_mapping_init(UmiDataResultMapping *item, const char *mapping_id, const char *entity_id, const char *field_name, uint32_t column_ordinal, UmiDataValueKind kind) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->mapping_id,sizeof(item->mapping_id),mapping_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->entity_id,sizeof(item->entity_id),entity_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->field_name,sizeof(item->field_name),field_name);if(s!=UMI_STATUS_OK)return s;item->column_ordinal=column_ordinal;item->kind=kind;
    return umi_data_result_mapping_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_result_mapping_validate(const UmiDataResultMapping *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->mapping_id[0] != '\0' && item->entity_id[0] != '\0' && item->field_name[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
