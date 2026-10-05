/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/converter.h
 *
 * PURPOSE:
 *   Describe a named value converter with forward and reverse availability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_CONVERTER_H
#define UMICOM_UI_REACTIVE_CONVERTER_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive converter data shared with callers of this public contract.
 */
typedef struct UmiUiReactiveConverter {
    char converter_id[UMI_UI_REACTIVE_ID_CAPACITY];
    UmiUiValueKind source_kind;
    UmiUiValueKind target_kind;
    bool supports_reverse;
} UmiUiReactiveConverter;
/**
 * Initialise ui reactive converter from caller-provided values so later operations receive
 * a known state.
 */
void umi_ui_reactive_converter_init(UmiUiReactiveConverter *item);
/**
 * Check that ui reactive converter satisfies its contract before another service relies on
 * it.
 */
int umi_ui_reactive_converter_valid(const UmiUiReactiveConverter *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_converter_archive_encode(const UmiUiReactiveConverter *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_converter_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveConverter *value);

#ifdef __cplusplus
}
#endif
#endif
