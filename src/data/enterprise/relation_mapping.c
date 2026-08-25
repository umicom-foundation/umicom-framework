/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/relation_mapping.c
 *
 * PURPOSE:
 *   Describe entity relations independently from SQL foreign-key syntax.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/relation_mapping.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_relation_mapping_init(UmiDataRelationMapping *item, const char *relation_id, const char *source_entity, const char *target_entity, const char *source_field, bool collection) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->relation_id,sizeof(item->relation_id),relation_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->source_entity,sizeof(item->source_entity),source_entity);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->target_entity,sizeof(item->target_entity),target_entity);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->source_field,sizeof(item->source_field),source_field);if(s!=UMI_STATUS_OK)return s;item->collection=collection;item->required=false;
    return umi_data_relation_mapping_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_relation_mapping_validate(const UmiDataRelationMapping *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->relation_id[0] != '\0' && item->source_entity[0] != '\0' && item->target_entity[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
