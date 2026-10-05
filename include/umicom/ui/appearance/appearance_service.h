/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/appearance_service.h
 *
 * PURPOSE:
 *   Expose aggregate readiness for Framework-owned production appearance services consumed by every thin application.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_APPEARANCE_SERVICE_H
#define UMICOM_UI_APPEARANCE_APPEARANCE_SERVICE_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance appearance service data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceAppearanceService {
    char service_id[UMI_APPEARANCE_ID_CAPACITY];
    bool themes_ready;
    bool typography_ready;
    bool scaling_ready;
    bool accessibility_ready;
    bool renderers_ready;
    uint64_t revision;
} UmiAppearanceAppearanceService;

/* Initialise one appearance service record with deterministic defaults. */
UmiStatus umi_appearance_service_init(UmiAppearanceAppearanceService *item);
/* Validate the required production invariants for this appearance service. */
int umi_appearance_service_is_valid(const UmiAppearanceAppearanceService *item);
/* Return one only when all production appearance capability groups are ready. */
int umi_appearance_service_ready(const UmiAppearanceAppearanceService *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_service_archive_encode(const UmiAppearanceAppearanceService *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_service_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceAppearanceService *value);

#ifdef __cplusplus
}
#endif
#endif
