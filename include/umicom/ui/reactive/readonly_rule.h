/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/readonly_rule.h
 *
 * PURPOSE:
 *   Bind editor read-only state to declarative state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_READONLY_RULE_H
#define UMICOM_UI_REACTIVE_READONLY_RULE_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive readonly rule data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveReadonlyRule {
    char target_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char expression[UMI_UI_REACTIVE_TEXT_CAPACITY];
    bool read_only;
} UmiUiReactiveReadonlyRule;
/**
 * Initialise ui reactive readonly rule from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_readonly_rule_init(UmiUiReactiveReadonlyRule *item);
/**
 * Check that ui reactive readonly rule satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_readonly_rule_valid(const UmiUiReactiveReadonlyRule *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_readonly_rule_archive_encode(const UmiUiReactiveReadonlyRule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_readonly_rule_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveReadonlyRule *value);

#ifdef __cplusplus
}
#endif
#endif
