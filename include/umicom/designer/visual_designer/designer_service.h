/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/designer_service.h
 *
 * PURPOSE:
 *   Aggregate visual designer readiness and active-session state for thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_DESIGNER_SERVICE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_DESIGNER_SERVICE_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer service data shared with callers of this public contract.
 */
typedef struct UmiRadDesignerService {
    size_t active_sessions;
    size_t open_documents;
    uint32_t conformance_score;
    bool initialized;
} UmiRadDesignerService;
/**
 * Initialise visual designer service from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_designer_service_init(UmiRadDesignerService *item);
/**
 * Check that visual designer service satisfies its contract before another service relies on
 * it.
 */
int umi_rad_designer_service_is_valid(const UmiRadDesignerService *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_designer_service_archive_encode(const UmiRadDesignerService *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_designer_service_archive_decode(const void *bytes, size_t byte_count,
    UmiRadDesignerService *value);

#ifdef __cplusplus
}
#endif
#endif
