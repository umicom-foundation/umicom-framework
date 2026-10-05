/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/accessibility_model.h
 *
 * PURPOSE:
 *   Describe accessible row/column metadata for virtualised enterprise views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_ACCESSIBILITY_MODEL_H
#define UMICOM_UI_ENTERPRISE_ACCESSIBILITY_MODEL_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent accessibility model data shared with callers of this public
 * contract.
 */
typedef struct UmiUiEntAccessibilityModel {
    char role[UMI_UI_ENT_ID_CAPACITY];
    char name[UMI_UI_ENT_TEXT_CAPACITY];
    char description[UMI_UI_ENT_TEXT_CAPACITY];
    size_t row_index;
    size_t column_index;
    size_t set_size;
    size_t position_in_set;
} UmiUiEntAccessibilityModel;
/**
 * Initialise ui ent accessibility model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_accessibility_model_init(UmiUiEntAccessibilityModel *value);
/**
 * Check that ui ent accessibility model satisfies its contract before another service
 * relies on it.
 */
int umi_ui_ent_accessibility_model_validate(const UmiUiEntAccessibilityModel *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_ent_accessibility_model_archive_encode(const UmiUiEntAccessibilityModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_ent_accessibility_model_archive_decode(const void *bytes, size_t byte_count,
    UmiUiEntAccessibilityModel *value);

#ifdef __cplusplus
}
#endif

#endif
