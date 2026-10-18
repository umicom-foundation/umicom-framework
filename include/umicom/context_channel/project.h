/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/project.h
 *
 * PURPOSE:
 *   Define the canonical project context shared by applications and panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_PROJECT_H
#define UMICOM_CONTEXT_CHANNEL_PROJECT_H
#include "umicom/context_channel/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project context data shared with callers of this public contract.
 */
typedef struct UmiProjectContext {
    uint32_t structure_size;
    char project_id[UMI_CONTEXT_TEXT_CAPACITY];
    char repository_id[UMI_CONTEXT_TEXT_CAPACITY];
    char root_path[UMI_CONTEXT_PATH_CAPACITY];
    char target_id[UMI_CONTEXT_TEXT_CAPACITY];
    char configuration_id[UMI_CONTEXT_TEXT_CAPACITY];
    char language_id[64U];
    uint64_t revision;
} UmiProjectContext;
/**
 * Initialise project context from caller-provided values so later operations receive a
 * known state.
 */
void umi_project_context_init(UmiProjectContext *context);
/**
 * Check that project context satisfies its contract before another service relies on it.
 */
UmiStatus umi_project_context_validate(const UmiProjectContext *context);
/**
 * Copy project context into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_project_context_copy(UmiProjectContext *destination, const UmiProjectContext *source);
/**
 * Provide the project context set project id operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_context_set_project_id(UmiProjectContext *context, const char *value);
/**
 * Provide the project context set repository id operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_context_set_repository_id(UmiProjectContext *context, const char *value);
/**
 * Provide the project context set root path operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_context_set_root_path(UmiProjectContext *context, const char *value);
/**
 * Provide the project context set target id operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_context_set_target_id(UmiProjectContext *context, const char *value);
/**
 * Provide the project context set configuration id operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_context_set_configuration_id(UmiProjectContext *context, const char *value);
/**
 * Provide the project context set language id operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_context_set_language_id(UmiProjectContext *context, const char *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_context_archive_encode(const UmiProjectContext *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_context_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectContext *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiProjectContext object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_project_context_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiProjectContext *workspace, UmiProjectContext *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_project_context_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiProjectContext *expected, UmiProjectContext *value);


#ifdef __cplusplus
}
#endif
#endif
