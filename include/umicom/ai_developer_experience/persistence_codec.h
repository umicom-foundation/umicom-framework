/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/persistence_codec.h
 *
 * PURPOSE:
 *   Encode multiline AI developer state into the existing single-line
 *   UmiSessionStore values using bounded hexadecimal chunks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_PERSISTENCE_CODEC_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_PERSISTENCE_CODEC_H

#include <stdint.h>
#include "umicom/platform/session_store.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_DEVELOPER_PERSISTENCE_CHUNK_BYTES \
    ((UMI_SESSION_VALUE_CAPACITY - 4U) / 2U)
#define UMI_AI_DEVELOPER_PERSISTENCE_MAX_CHUNKS 32U

UmiStatus umi_ai_developer_persistence_save_text(
    UmiSessionStore *store,
    const char *key_prefix,
    const char *text,
    size_t length,
    size_t *out_chunk_count);

UmiStatus umi_ai_developer_persistence_load_text(
    const UmiSessionStore *store,
    const char *key_prefix,
    size_t chunk_count,
    char *out_text,
    size_t capacity,
    size_t *out_length);

UmiStatus umi_ai_developer_persistence_remove_text(
    UmiSessionStore *store,
    const char *key_prefix,
    size_t chunk_count);

UmiStatus umi_ai_developer_persistence_set_uint64(
    UmiSessionStore *store,
    const char *key,
    uint64_t value);

UmiStatus umi_ai_developer_persistence_get_uint64(
    const UmiSessionStore *store,
    const char *key,
    uint64_t default_value,
    uint64_t *out_value);

#ifdef __cplusplus
}
#endif
#endif
