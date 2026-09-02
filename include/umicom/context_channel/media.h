/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/media.h
 *
 * PURPOSE:
 *   Define the canonical media context shared by applications and panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_MEDIA_H
#define UMICOM_CONTEXT_CHANNEL_MEDIA_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the media context data shared with callers of this public contract.
 */
typedef struct UmiMediaContext {
    uint32_t structure_size;
    char asset_id[UMI_CONTEXT_TEXT_CAPACITY];
    char timeline_id[UMI_CONTEXT_TEXT_CAPACITY];
    char track_id[UMI_CONTEXT_TEXT_CAPACITY];
    uint64_t timecode_ms;
    uint64_t duration_ms;
    char media_type[64U];
    uint64_t revision;
} UmiMediaContext;
/**
 * Initialise media context from caller-provided values so later operations receive a known
 * state.
 */
void umi_media_context_init(UmiMediaContext *context);
/**
 * Check that media context satisfies its contract before another service relies on it.
 */
UmiStatus umi_media_context_validate(const UmiMediaContext *context);
/**
 * Copy media context into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_media_context_copy(UmiMediaContext *destination, const UmiMediaContext *source);
/**
 * Provide the media context set asset id operation used by this module and its client
 * applications.
 */
UmiStatus umi_media_context_set_asset_id(UmiMediaContext *context, const char *value);
/**
 * Provide the media context set timeline id operation used by this module and its client
 * applications.
 */
UmiStatus umi_media_context_set_timeline_id(UmiMediaContext *context, const char *value);
/**
 * Provide the media context set track id operation used by this module and its client
 * applications.
 */
UmiStatus umi_media_context_set_track_id(UmiMediaContext *context, const char *value);
/**
 * Provide the media context set timecode ms operation used by this module and its client
 * applications.
 */
UmiStatus umi_media_context_set_timecode_ms(UmiMediaContext *context, uint64_t value);
/**
 * Provide the media context set duration ms operation used by this module and its client
 * applications.
 */
UmiStatus umi_media_context_set_duration_ms(UmiMediaContext *context, uint64_t value);
/**
 * Provide the media context set media type operation used by this module and its client
 * applications.
 */
UmiStatus umi_media_context_set_media_type(UmiMediaContext *context, const char *value);
#ifdef __cplusplus
}
#endif
#endif
