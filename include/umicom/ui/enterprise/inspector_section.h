/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/inspector_section.h
 *
 * PURPOSE:
 *   Describe a collapsible property-inspector section.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_INSPECTOR_SECTION_H
#define UMICOM_UI_ENTERPRISE_INSPECTOR_SECTION_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent inspector section data shared with callers of this public contract.
 */
typedef struct UmiUiEntInspectorSection {
    char section_id[UMI_UI_ENT_ID_CAPACITY];
    char label[UMI_UI_ENT_TEXT_CAPACITY];
    int32_t order;
    int collapsed;
} UmiUiEntInspectorSection;
/**
 * Initialise ui ent inspector section from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_inspector_section_init(UmiUiEntInspectorSection *value);
/**
 * Check that ui ent inspector section satisfies its contract before another service relies
 * on it.
 */
int umi_ui_ent_inspector_section_validate(const UmiUiEntInspectorSection *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_ent_inspector_section_archive_encode(const UmiUiEntInspectorSection *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_ent_inspector_section_archive_decode(const void *bytes, size_t byte_count,
    UmiUiEntInspectorSection *value);

#ifdef __cplusplus
}
#endif

#endif
