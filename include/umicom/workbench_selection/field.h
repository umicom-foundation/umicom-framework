/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/field.h
 *
 * PURPOSE:
 *   Define named typed fields carried by structured workbench selections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_FIELD_H
#define UMICOM_WORKBENCH_SELECTION_FIELD_H

#include "umicom/workbench_selection/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection field data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionField {
    char name[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    UmiWorkbenchSelectionValueKind kind;
    char text[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    int64_t integer_value;
    uint64_t unsigned_value;
    double decimal_value;
    bool boolean_value;
} UmiWorkbenchSelectionField;

/**
 * Initialise workbench selection field from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_selection_field_init(
    UmiWorkbenchSelectionField *field,
    const char *name);
/**
 * Provide the workbench selection field set text operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_field_set_text(
    UmiWorkbenchSelectionField *field,
    const char *value);
/**
 * Provide the workbench selection field set integer operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_field_set_integer(
    UmiWorkbenchSelectionField *field,
    int64_t value);
/**
 * Provide the workbench selection field set unsigned operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_field_set_unsigned(
    UmiWorkbenchSelectionField *field,
    uint64_t value);
/**
 * Provide the workbench selection field set decimal operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_field_set_decimal(
    UmiWorkbenchSelectionField *field,
    double value);
/**
 * Provide the workbench selection field set boolean operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_field_set_boolean(
    UmiWorkbenchSelectionField *field,
    bool value);
/**
 * Check that workbench selection field satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_selection_field_validate(
    const UmiWorkbenchSelectionField *field);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_selection_field_archive_encode(const UmiWorkbenchSelectionField *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_selection_field_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionField *value);

#ifdef __cplusplus
}
#endif
#endif
