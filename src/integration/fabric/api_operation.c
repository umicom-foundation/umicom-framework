/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/api_operation.c
 *
 * PURPOSE:
 *   Describe one API operation including method, route, schemas and idempotency semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/api_operation.h"
#include <string.h>
#include <limits.h>

UmiStatus umi_fabric_api_operation_init(UmiFabricApiOperation *item, const char *operation_id, const char *method, const char *path, const char *request_schema, const char *response_schema, bool idempotent) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->operation_id,sizeof(item->operation_id),operation_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->method,sizeof(item->method),method);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->path,sizeof(item->path),path);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->request_schema,sizeof(item->request_schema),request_schema);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->response_schema,sizeof(item->response_schema),response_schema);if(s!=UMI_STATUS_OK)return s;item->idempotent=idempotent;
    return umi_fabric_api_operation_validate(item);
}
UmiStatus umi_fabric_api_operation_validate(const UmiFabricApiOperation *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->operation_id[0]!='\0' && item->method[0]!='\0' && item->path[0]=='/')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
