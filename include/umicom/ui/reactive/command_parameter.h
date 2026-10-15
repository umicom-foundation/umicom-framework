/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/command_parameter.h
 *
 * PURPOSE:
 *   Represent a revisioned command parameter value.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_COMMAND_PARAMETER_H
#define UMICOM_UI_REACTIVE_COMMAND_PARAMETER_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive command parameter data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveCommandParameter {
    char name[UMI_UI_REACTIVE_ID_CAPACITY];
    UmiUiValue value;
    uint64_t revision;
} UmiUiReactiveCommandParameter;
/**
 * Initialise ui reactive command parameter from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_command_parameter_init(UmiUiReactiveCommandParameter *item);
/**
 * Check that ui reactive command parameter satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_command_parameter_valid(const UmiUiReactiveCommandParameter *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_command_parameter_archive_encode(const UmiUiReactiveCommandParameter *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_command_parameter_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveCommandParameter *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ui_reactive_command_parameter_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUiReactiveCommandParameter *expected, UmiUiReactiveCommandParameter *value);


#ifdef __cplusplus
}
#endif
#endif
