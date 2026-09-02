/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/key_codec.h
 *
 * PURPOSE:
 *   Build and parse bounded Data Server keys for layouts, sessions, revisions, sharing, synchronisation and recovery records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_KEY_CODEC_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_KEY_CODEC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/workbench_layout_data/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench layout data key parts data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutDataKeyParts {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordKind kind;
    char aggregate_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char record_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    uint64_t sequence;
    size_t chunk_index;
} UmiWorkbenchLayoutDataKeyParts;

/**
 * Provide the workbench layout data key build operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_data_key_build(
    UmiWorkbenchLayoutDataRecordKind kind,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    size_t chunk_index,
    char *buffer,
    size_t capacity);

/**
 * Read workbench layout data key into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_data_key_parse(
    const char *key,
    UmiWorkbenchLayoutDataKeyParts *out_parts);

/**
 * Provide the workbench layout data key prefix operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_data_key_prefix(
    UmiWorkbenchLayoutDataRecordKind kind,
    const char *aggregate_id,
    char *buffer,
    size_t capacity);

/**
 * Provide the workbench layout data key is owned operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_data_key_is_owned(
    const char *key);

/**
 * Provide the workbench layout data key matches prefix operation used by this module and
 * its client applications.
 */
bool umi_workbench_layout_data_key_matches_prefix(
    const char *key,
    const char *prefix);

#ifdef __cplusplus
}
#endif

#endif
