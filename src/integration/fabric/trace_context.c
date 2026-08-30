/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/trace_context.c
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
#include "umicom/integration/fabric/trace_context.h"
#include <string.h>
#include <limits.h>


UmiStatus umi_fabric_trace_context_root(UmiFabricTraceContext *c,const char *trace_id,const char *span_id,const char *correlation_id,bool sampled){UmiStatus s;if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(c,0,sizeof(*c));s=umi_fabric_copy_text(c->trace_id,sizeof(c->trace_id),trace_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(c->span_id,sizeof(c->span_id),span_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(c->correlation_id,sizeof(c->correlation_id),correlation_id);if(s!=UMI_STATUS_OK)return s;c->sampled=sampled;return UMI_STATUS_OK;}
UmiStatus umi_fabric_trace_context_child(const UmiFabricTraceContext *p,const char *child_span_id,UmiFabricTraceContext *c){UmiStatus s;if(p==NULL||c==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(c,0,sizeof(*c));s=umi_fabric_copy_text(c->trace_id,sizeof(c->trace_id),p->trace_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(c->span_id,sizeof(c->span_id),child_span_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(c->parent_span_id,sizeof(c->parent_span_id),p->span_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(c->correlation_id,sizeof(c->correlation_id),p->correlation_id);if(s!=UMI_STATUS_OK)return s;c->sampled=p->sampled;return UMI_STATUS_OK;}
