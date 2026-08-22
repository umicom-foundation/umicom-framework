/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/query.h
 *
 * PURPOSE:
 *   Define reusable context-host query requests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_QUERY_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_QUERY_H
#include "umicom/workbench_context_host/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostQuery {
    uint32_t structure_size;
    char query_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextHostQueryKind kind;
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    uint64_t requested_at_ms;
} UmiWorkbenchContextHostQuery;
void umi_workbench_context_host_query_init(
    UmiWorkbenchContextHostQuery *query,UmiWorkbenchContextHostQueryKind kind);
#ifdef __cplusplus
}
#endif
#endif
