/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/required_rule.h
 *
 * PURPOSE:
 *   Bind form required-state to declarative state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_REQUIRED_RULE_H
#define UMICOM_UI_REACTIVE_REQUIRED_RULE_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive required rule data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveRequiredRule {
    char target_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char expression[UMI_UI_REACTIVE_TEXT_CAPACITY];
    bool required;
} UmiUiReactiveRequiredRule;
/**
 * Initialise ui reactive required rule from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_required_rule_init(UmiUiReactiveRequiredRule *item);
/**
 * Check that ui reactive required rule satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_required_rule_valid(const UmiUiReactiveRequiredRule *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_required_rule_archive_encode(const UmiUiReactiveRequiredRule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_required_rule_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveRequiredRule *value);

#ifdef __cplusplus
}
#endif
#endif
