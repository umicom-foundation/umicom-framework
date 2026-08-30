/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/schema_descriptor.c
 *
 * PURPOSE:
 *   Describe a versioned integration schema and compatibility intent independently of serialization format.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/schema_descriptor.h"
#include <string.h>
#include <limits.h>

UmiStatus umi_fabric_schema_descriptor_init(UmiFabricSchemaDescriptor *item, const char *schema_id, const char *name, UmiFabricVersion version, uint64_t fingerprint, bool backward_compatible, bool forward_compatible) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->schema_id,sizeof(item->schema_id),schema_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->name,sizeof(item->name),name);if(s!=UMI_STATUS_OK)return s;item->version=version;item->fingerprint=fingerprint;item->backward_compatible=backward_compatible;item->forward_compatible=forward_compatible;
    return umi_fabric_schema_descriptor_validate(item);
}
UmiStatus umi_fabric_schema_descriptor_validate(const UmiFabricSchemaDescriptor *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->schema_id[0]!='\0' && item->name[0]!='\0' && item->version.major>0U && item->fingerprint!=0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
