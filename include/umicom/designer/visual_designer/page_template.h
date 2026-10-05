/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/page_template.h
 *
 * PURPOSE:
 *   Describe reusable page templates without embedding application-specific logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_PAGE_TEMPLATE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_PAGE_TEMPLATE_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer page template data shared with callers of this public contract.
 */
typedef struct UmiRadPageTemplate {
    char template_id[UMI_RAD_ID_CAPACITY];
    char name[UMI_RAD_TEXT_CAPACITY];
    char shell_kind[UMI_RAD_ID_CAPACITY];
    size_t initial_components;
} UmiRadPageTemplate;
/**
 * Initialise visual designer page template from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_page_template_init(UmiRadPageTemplate *item);
/**
 * Check that visual designer page template satisfies its contract before another service relies on it.
 */
int umi_rad_page_template_is_valid(const UmiRadPageTemplate *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_page_template_archive_encode(const UmiRadPageTemplate *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_page_template_archive_decode(const void *bytes, size_t byte_count,
    UmiRadPageTemplate *value);

#ifdef __cplusplus
}
#endif
#endif
