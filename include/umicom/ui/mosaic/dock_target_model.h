/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/dock_target_model.h
 *
 * PURPOSE:
 *   Define toolkit-neutral dock target model contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_DOCK_TARGET_MODEL_H
#define UMICOM_UI_MOSAIC_DOCK_TARGET_MODEL_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic dock target model data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicDockTargetModel {
    char source_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char target_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicDockZone zone;
    uint32_t sequence;
    bool allowed;
} UmiUiMosaicDockTargetModel;

/* Initializes one dock target model relation. */
void umi_ui_mosaic_dock_target_model_init(UmiUiMosaicDockTargetModel *value);
/* Configures source/target/zone without performing a renderer mutation. */
UmiStatus umi_ui_mosaic_dock_target_model_set(UmiUiMosaicDockTargetModel *value, const char *source_id, const char *target_id, UmiUiMosaicDockZone zone);
/* Rejects malformed or self-referential dock operations. */
UmiStatus umi_ui_mosaic_dock_target_model_validate(const UmiUiMosaicDockTargetModel *value);
/* Returns whether the relation is a centre/tab-style target. */
int umi_ui_mosaic_dock_target_model_is_centre(const UmiUiMosaicDockTargetModel *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_dock_target_model_archive_encode(const UmiUiMosaicDockTargetModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_dock_target_model_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicDockTargetModel *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiUiMosaicDockTargetModel object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_ui_mosaic_dock_target_model_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiUiMosaicDockTargetModel *workspace, UmiUiMosaicDockTargetModel *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ui_mosaic_dock_target_model_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUiMosaicDockTargetModel *expected, UmiUiMosaicDockTargetModel *value);


#ifdef __cplusplus
}
#endif
#endif
