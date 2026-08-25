/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/trace_context.h
 *
 * PURPOSE:
 *   Propagate trace/span/correlation identity without owning the observability transport.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_TRACE_CONTEXT_H
#define UMICOM_INTEGRATION_FABRIC_TRACE_CONTEXT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricTraceContext { char trace_id[UMI_FABRIC_ID_CAPACITY]; char span_id[UMI_FABRIC_ID_CAPACITY]; char parent_span_id[UMI_FABRIC_ID_CAPACITY]; char correlation_id[UMI_FABRIC_ID_CAPACITY]; bool sampled; } UmiFabricTraceContext;
UmiStatus umi_fabric_trace_context_root(UmiFabricTraceContext *context,const char *trace_id,const char *span_id,const char *correlation_id,bool sampled);
UmiStatus umi_fabric_trace_context_child(const UmiFabricTraceContext *parent,const char *child_span_id,UmiFabricTraceContext *out_child);

#ifdef __cplusplus
}
#endif
#endif
