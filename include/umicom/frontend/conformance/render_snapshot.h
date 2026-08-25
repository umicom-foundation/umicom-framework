/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/render_snapshot.h
 *
 * PURPOSE:
 *   normalised semantic render snapshot used for frontend regression testing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_RENDER_SNAPSHOT_H
#define UMICOM_FRONTEND_CONFORMANCE_RENDER_SNAPSHOT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcRenderSnapshot { char surface_id[UMI_FC_ID_CAPACITY]; size_t node_count; size_t text_bytes; size_t interactive_count; uint64_t semantic_fingerprint; uint64_t revision; } UmiFcRenderSnapshot;
UmiStatus umi_fc_render_snapshot_make(const char *surface_id,size_t node_count,size_t text_bytes,size_t interactive_count,uint64_t semantic_fingerprint,uint64_t revision,UmiFcRenderSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
