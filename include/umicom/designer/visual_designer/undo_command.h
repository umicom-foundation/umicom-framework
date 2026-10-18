/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/undo_command.h
 *
 * PURPOSE:
 *   Represent a reversible designer mutation without toolkit dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_UNDO_COMMAND_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_UNDO_COMMAND_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer undo command data shared with callers of this public contract.
 */
typedef struct UmiRadUndoCommand {
    char command_id[UMI_RAD_ID_CAPACITY];
    char target_id[UMI_RAD_ID_CAPACITY];
    char before_value[UMI_RAD_VALUE_CAPACITY];
    char after_value[UMI_RAD_VALUE_CAPACITY];
} UmiRadUndoCommand;
/**
 * Initialise visual designer undo command from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_undo_command_init(UmiRadUndoCommand *item);
/**
 * Check that visual designer undo command satisfies its contract before another service relies on it.
 */
int umi_rad_undo_command_is_valid(const UmiRadUndoCommand *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_undo_command_archive_encode(const UmiRadUndoCommand *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_undo_command_archive_decode(const void *bytes, size_t byte_count,
    UmiRadUndoCommand *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiRadUndoCommand object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_rad_undo_command_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiRadUndoCommand *workspace, UmiRadUndoCommand *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_rad_undo_command_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiRadUndoCommand *expected, UmiRadUndoCommand *value);


#ifdef __cplusplus
}
#endif
#endif
