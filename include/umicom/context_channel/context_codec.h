/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_codec.h
 *
 * PURPOSE:
 *   Describe codec selection for local IPC, JSON and future binary transports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_CODEC_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_CODEC_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context codec data shared with callers of this public contract.
 */
typedef struct UmiContextCodec {
    uint32_t structure_size;
    char codec_id[UMI_CONTEXT_VALUE_CAPACITY];
    char schema_id[UMI_CONTEXT_VALUE_CAPACITY];
    char media_type[UMI_CONTEXT_VALUE_CAPACITY];
    char encoding[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextCodec;
/**
 * Initialise context codec from caller-provided values so later operations receive a known
 * state.
 */
void umi_context_codec_init(UmiContextCodec *state);
/**
 * Provide the context codec set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_codec_set_field(UmiContextCodec *state,size_t field_index,const char *value);
/**
 * Provide the context codec field operation used by this module and its client
 * applications.
 */
const char *umi_context_codec_field(const UmiContextCodec *state,size_t field_index);
/**
 * Provide the context codec record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_codec_record_success(UmiContextCodec *state,uint64_t sequence);
/**
 * Provide the context codec record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_codec_record_failure(UmiContextCodec *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context codec satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_codec_validate(const UmiContextCodec *state);
/**
 * Provide the context codec covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_codec_covers_sequence(const UmiContextCodec *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
