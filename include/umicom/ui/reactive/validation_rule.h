/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/validation_rule.h
 *
 * PURPOSE:
 *   Represent common required/range/length validation constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_VALIDATION_RULE_H
#define UMICOM_UI_REACTIVE_VALIDATION_RULE_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive validation rule data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveValidationRule {
    char rule_id[UMI_UI_REACTIVE_ID_CAPACITY];
    bool required;
    double minimum;
    double maximum;
    size_t min_length;
    size_t max_length;
} UmiUiReactiveValidationRule;
/**
 * Initialise ui reactive validation rule from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_validation_rule_init(UmiUiReactiveValidationRule *item);
/**
 * Check that ui reactive validation rule satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_validation_rule_valid(const UmiUiReactiveValidationRule *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_validation_rule_archive_encode(const UmiUiReactiveValidationRule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_validation_rule_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveValidationRule *value);

#ifdef __cplusplus
}
#endif
#endif
