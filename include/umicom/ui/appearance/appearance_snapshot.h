/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/appearance_snapshot.h
 *
 * PURPOSE:
 *   Persist resolved appearance identity and revisions for deterministic session restore and visual tests.
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
#ifndef UMICOM_UI_APPEARANCE_APPEARANCE_SNAPSHOT_H
#define UMICOM_UI_APPEARANCE_APPEARANCE_SNAPSHOT_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance appearance snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceAppearanceSnapshot {
    char snapshot_id[UMI_APPEARANCE_ID_CAPACITY];
    char profile_id[UMI_APPEARANCE_ID_CAPACITY];
    char theme_pack_id[UMI_APPEARANCE_ID_CAPACITY];
    double effective_scale;
    uint64_t semantic_revision;
    uint64_t fingerprint;
} UmiAppearanceAppearanceSnapshot;

/* Initialise one appearance snapshot record with deterministic defaults. */
UmiStatus umi_appearance_snapshot_init(UmiAppearanceAppearanceSnapshot *item);
/* Validate the required production invariants for this appearance snapshot. */
int umi_appearance_snapshot_is_valid(const UmiAppearanceAppearanceSnapshot *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_snapshot_archive_encode(const UmiAppearanceAppearanceSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceAppearanceSnapshot *value);

#ifdef __cplusplus
}
#endif
#endif
