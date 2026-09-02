/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/value_codec.h
 *
 * PURPOSE:
 *   Encode and decode bounded self-describing Data Server values without exposing database-specific representations to callers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_VALUE_CODEC_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_VALUE_CODEC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/workbench_layout_data/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench layout data field data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutDataField {
    char name[64];
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
} UmiWorkbenchLayoutDataField;

/**
 * Represent the workbench layout data field set data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutDataFieldSet {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataField fields[64];
    size_t count;
} UmiWorkbenchLayoutDataFieldSet;

/**
 * Initialise workbench layout data field set from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_layout_data_field_set_init(
    UmiWorkbenchLayoutDataFieldSet *fields);

/**
 * Provide the workbench layout data field set put operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_data_field_set_put(
    UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    const char *value);

/**
 * Provide the workbench layout data field set put u64 operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_field_set_put_u64(
    UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    uint64_t value);

/**
 * Provide the workbench layout data field set put u32 operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_field_set_put_u32(
    UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    uint32_t value);

/**
 * Provide the workbench layout data field set put bool operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_field_set_put_bool(
    UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    bool value);

/**
 * Provide the workbench layout data field set get operation used by this module and its
 * client applications.
 */
const char *umi_workbench_layout_data_field_set_get(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name);

/**
 * Provide the workbench layout data field set get u64 operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_field_set_get_u64(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    uint64_t *out_value);

/**
 * Provide the workbench layout data field set get u32 operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_field_set_get_u32(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    uint32_t *out_value);

/**
 * Provide the workbench layout data field set get bool operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_field_set_get_bool(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    bool *out_value);

/**
 * Write workbench layout data value in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_data_value_encode(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    char *buffer,
    size_t capacity,
    size_t *out_required);

/**
 * Read workbench layout data value into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_data_value_decode(
    const char *value,
    UmiWorkbenchLayoutDataFieldSet *out_fields);

/**
 * Provide the workbench layout data value escape operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_data_value_escape(
    const char *text,
    char *buffer,
    size_t capacity,
    size_t *out_required);

/**
 * Provide the workbench layout data value unescape operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_data_value_unescape(
    const char *text,
    char *buffer,
    size_t capacity,
    size_t *out_required);

#ifdef __cplusplus
}
#endif

#endif
