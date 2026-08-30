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
void umi_context_codec_init(UmiContextCodec *state);
UmiStatus umi_context_codec_set_field(UmiContextCodec *state,size_t field_index,const char *value);
const char *umi_context_codec_field(const UmiContextCodec *state,size_t field_index);
UmiStatus umi_context_codec_record_success(UmiContextCodec *state,uint64_t sequence);
UmiStatus umi_context_codec_record_failure(UmiContextCodec *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_codec_validate(const UmiContextCodec *state);
bool umi_context_codec_covers_sequence(const UmiContextCodec *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
