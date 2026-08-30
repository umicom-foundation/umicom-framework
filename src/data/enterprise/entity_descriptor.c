/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/entity_descriptor.c
 *
 * PURPOSE:
 *   Describe an ORM entity without coupling persistence mapping to application structs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/entity_descriptor.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_entity_descriptor_init(UmiDataEntityDescriptor *item, const char *entity_id, const char *table_id, const char *identity_field) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->entity_id,sizeof(item->entity_id),entity_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->table_id,sizeof(item->table_id),table_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->identity_field,sizeof(item->identity_field),identity_field);if(s!=UMI_STATUS_OK)return s;item->field_count=0U;item->immutable=false;
    return umi_data_entity_descriptor_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_entity_descriptor_validate(const UmiDataEntityDescriptor *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->entity_id[0] != '\0' && item->table_id[0] != '\0' && item->identity_field[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
