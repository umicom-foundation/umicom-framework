/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/trace_context.h
 *
 * PURPOSE:
 *   Propagate trace/span/correlation identity without owning the observability transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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


/**
 * Represent the fabric trace context data shared with callers of this public contract.
 */
typedef struct UmiFabricTraceContext { char trace_id[UMI_FABRIC_ID_CAPACITY]; char span_id[UMI_FABRIC_ID_CAPACITY]; char parent_span_id[UMI_FABRIC_ID_CAPACITY]; char correlation_id[UMI_FABRIC_ID_CAPACITY]; bool sampled; } UmiFabricTraceContext;
/**
 * Provide the fabric trace context root operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_trace_context_root(UmiFabricTraceContext *context,const char *trace_id,const char *span_id,const char *correlation_id,bool sampled);
/**
 * Provide the fabric trace context child operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_trace_context_child(const UmiFabricTraceContext *parent,const char *child_span_id,UmiFabricTraceContext *out_child);

#ifdef __cplusplus
}
#endif
#endif
