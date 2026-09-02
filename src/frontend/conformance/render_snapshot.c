/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/render_snapshot.c
 *
 * PURPOSE:
 *   normalised semantic render snapshot used for frontend regression testing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/render_snapshot.h"

/*
 * Provide the fc render snapshot make operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_render_snapshot_make(const char *surface_id,size_t node_count,size_t text_bytes,size_t interactive_count,uint64_t semantic_fingerprint,uint64_t revision,UmiFcRenderSnapshot *out_snapshot){UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_snapshot==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_snapshot=(UmiFcRenderSnapshot){0};st=umi_fc_copy_text(out_snapshot->surface_id,sizeof(out_snapshot->surface_id),surface_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;out_snapshot->node_count=node_count;out_snapshot->text_bytes=text_bytes;out_snapshot->interactive_count=interactive_count;out_snapshot->semantic_fingerprint=semantic_fingerprint;out_snapshot->revision=revision;return UMI_STATUS_OK;}
