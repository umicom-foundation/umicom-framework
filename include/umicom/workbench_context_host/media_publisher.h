/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/media_publisher.h
 *
 * PURPOSE:
 *   Publish canonical media contexts for future audio, video and generated-content workbenches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_MEDIA_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_MEDIA_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the workbench context host publish media operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_publish_media(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * asset_id,
    const char * timeline_id,
    const char * track_id,
    uint64_t timecode_ms,
    uint64_t duration_ms,
    const char * media_type,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
