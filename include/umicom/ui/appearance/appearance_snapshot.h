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

#ifdef __cplusplus
}
#endif
#endif
