/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/coding_patch.c
 *
 * PURPOSE:
 *   Implement bounded patch previews, approval, conflict detection, controlled
 *   application, failure rollback and later user-requested reversion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Applying a patch is a small transaction.  Every current file is checked
 * against the reviewed before-hash before the first write, and completed writes
 * are rolled back if a later callback fails.
 */
#include "umicom/ai/coding_patch.h"
#include "umicom/base/snapshot_validation.h"
#include "../base/value_construction_internal.h"

#include <stdio.h>
#include <string.h>

/* Keep the public patch value layout stable. Stack frames admit nested work on
 * a different patch while refusing mutation of any outer borrowed patch.
 * This detects synchronous callback reentry, not concurrent use by threads. */
typedef struct CodingPatchTransactionFrame {
    const UmiAiCodingPatch *patch;
    struct CodingPatchTransactionFrame *previous;
} CodingPatchTransactionFrame;
static _Thread_local CodingPatchTransactionFrame *codingPatchTransactions;

UmiStatus UmiAiCodingPatchCheckIdle(const UmiAiCodingPatch *patch)
{
    if (patch == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (const CodingPatchTransactionFrame *frame = codingPatchTransactions;
         frame != NULL; frame = frame->previous)
        if (frame->patch == patch) return UMI_STATUS_BUSY;
    return UMI_STATUS_OK;
}


/* Provide the copy text operation used by this module and its client applications. */
static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) return 0;
    written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

/* Return the number of records represented by line without changing their state. */
static uint32_t line_count(const char *text, size_t length)
{
    uint32_t lines = length > 0U ? 1U : 0U;
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (text[index] == '\n' && index + 1U < length) ++lines;
    }
    return lines;
}

/*
 * Provide the ai coding patch policy default operation used by this module and its client
 * applications.
 */
UmiAiCodingPatchPolicy umi_ai_coding_patch_policy_default(void)
{
    UmiAiCodingPatchPolicy policy;
    policy.maximum_files = UMI_AI_CODING_PATCH_FILE_MAX;
    policy.maximum_changed_lines = 2000U;
    policy.allow_create = 1;
    policy.allow_delete = 0;
    policy.require_approval = 1;
    return policy;
}

/* Framework owns physical limits independently of caller policy. */
UmiStatus UmiAiCodingPatchPolicyValidate(const UmiAiCodingPatchPolicy *policy)
{
    if (policy == NULL || policy->maximum_files == 0U ||
        policy->maximum_files > UMI_AI_CODING_PATCH_FILE_MAX ||
        policy->maximum_changed_lines == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* External inputs provide readable storage through their terminator or this
 * limit. Inline fields are checked within their declared array capacity. */
static size_t patch_text_length(const char *text, size_t capacity)
{
    if (text == NULL) return capacity;
    for (size_t i = 0U; i < capacity; ++i)
        if (text[i] == '\0') return i;
    return capacity;
}

UmiStatus UmiAiCodingPatchCheckIntegrity(const UmiAiCodingPatch *patch)
{
    static const UmiSnapshotTextField fields[] = {
        {"patch_id", offsetof(UmiAiCodingPatch, patch_id), sizeof(((UmiAiCodingPatch *)0)->patch_id), 1},
        {"request_id", offsetof(UmiAiCodingPatch, request_id), sizeof(((UmiAiCodingPatch *)0)->request_id), 1},
        {"title", offsetof(UmiAiCodingPatch, title), sizeof(((UmiAiCodingPatch *)0)->title), 1},
        {"rationale", offsetof(UmiAiCodingPatch, rationale), sizeof(((UmiAiCodingPatch *)0)->rationale), 1},
        {"approved_by", offsetof(UmiAiCodingPatch, approved_by), sizeof(((UmiAiCodingPatch *)0)->approved_by), 0}
    };
    uint32_t changed = 0U;
    if (patch == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (patch->structure_size < sizeof *patch ||
        patch->abi_version != UMI_AI_CODING_ABI_VERSION ||
        patch->file_count > UMI_AI_CODING_PATCH_FILE_MAX ||
        patch->revision == 0U ||
        patch->state < UMI_AI_CODING_PATCH_DRAFT ||
        patch->state > UMI_AI_CODING_PATCH_REJECTED ||
        UmiSnapshotValidateTextFields(patch, sizeof *patch, fields,
            sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        (patch->state == UMI_AI_CODING_PATCH_APPROVED && patch->approved_by[0] == '\0'))
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < patch->file_count; ++i) {
        const UmiAiCodingPatchFile *file = &patch->files[i];
        if (patch_text_length(file->path, sizeof file->path) == sizeof file->path)
            return UMI_STATUS_INVALID_STATE;
        if (!umi_ai_coding_path_is_safe_relative(file->path))
            return UMI_STATUS_PERMISSION_DENIED;
        if (file->operation < UMI_AI_CODING_PATCH_CREATE ||
            file->operation > UMI_AI_CODING_PATCH_DELETE ||
            file->before_length >= sizeof file->before_text ||
            file->after_length >= sizeof file->after_text ||
            patch_text_length(file->before_text, sizeof file->before_text) != file->before_length ||
            patch_text_length(file->after_text, sizeof file->after_text) != file->after_length ||
            umi_ai_coding_text_hash(file->before_text, file->before_length) != file->before_hash ||
            umi_ai_coding_text_hash(file->after_text, file->after_length) != file->after_hash ||
            line_count(file->before_text, file->before_length) != file->removed_lines ||
            line_count(file->after_text, file->after_length) != file->added_lines ||
            (file->operation == UMI_AI_CODING_PATCH_CREATE && file->before_length != 0U) ||
            (file->operation == UMI_AI_CODING_PATCH_DELETE && file->after_length != 0U))
            return UMI_STATUS_INVALID_STATE;
        /* Only compare against earlier records whose text was admitted. */
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(file->path, patch->files[j].path) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        if (UINT32_MAX - changed < file->removed_lines) return UMI_STATUS_INVALID_STATE;
        changed += file->removed_lines;
        if (UINT32_MAX - changed < file->added_lines) return UMI_STATUS_INVALID_STATE;
        changed += file->added_lines;
    }
    return changed == patch->changed_lines ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/*
 * Initialise ai coding patch from caller-provided values so later operations receive a
 * known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_ai_coding_patch_init(UmiAiCodingPatch *patch,
                                   const char *patch_id,
                                   const char *request_id,
                                   const char *title,
                                   const char *rationale)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (patch == NULL || patch_id == NULL || patch_id[0] == '\0' ||
        request_id == NULL || request_id[0] == '\0' || title == NULL ||
        title[0] == '\0' || rationale == NULL || rationale[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(patch, 0, sizeof(*patch));
    patch->structure_size = (uint32_t)sizeof(*patch);
    patch->abi_version = UMI_AI_CODING_ABI_VERSION;
    patch->state = UMI_AI_CODING_PATCH_DRAFT;
    patch->revision = 1U;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!copy_text(patch->patch_id, sizeof(patch->patch_id), patch_id) ||
        !copy_text(patch->request_id, sizeof(patch->request_id), request_id) ||
        !copy_text(patch->title, sizeof(patch->title), title) ||
        !copy_text(patch->rationale, sizeof(patch->rationale), rationale)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_ai_coding_patch_init_build_value(UmiAiCodingPatch *patch,
                                   const char *patch_id,
                                   const char *request_id,
                                   const char *title,
                                   const char *rationale)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (patch == NULL || patch_id == NULL || patch_id[0] == '\0' ||
        request_id == NULL || request_id[0] == '\0' || title == NULL ||
        title[0] == '\0' || rationale == NULL || rationale[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(patch, 0, sizeof(*patch));
    patch->structure_size = (uint32_t)sizeof(*patch);
    patch->abi_version = UMI_AI_CODING_ABI_VERSION;
    patch->state = UMI_AI_CODING_PATCH_DRAFT;
    patch->revision = 1U;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!copy_text(patch->patch_id, sizeof(patch->patch_id), patch_id) ||
        !copy_text(patch->request_id, sizeof(patch->request_id), request_id) ||
        !copy_text(patch->title, sizeof(patch->title), title) ||
        !copy_text(patch->rationale, sizeof(patch->rationale), rationale)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/* Construction now checks the live callback transaction before allocating or replacing a patch; staged failure isolation remains unchanged. The former implementation is retained for owner review. */
#if 0
UMI_DEFINE_STAGED_VALUE_INIT(umi_ai_coding_patch_init, UmiAiCodingPatch, patch,
    (UmiAiCodingPatch *patch, const char *patch_id, const char *request_id, const char *title, const char *rationale),
    umi_ai_coding_patch_init_build_value(candidate, patch_id, request_id, title, rationale))
#endif
static UMI_DEFINE_STAGED_VALUE_INIT(coding_patch_init_staged, UmiAiCodingPatch, patch,
    (UmiAiCodingPatch *patch, const char *patch_id, const char *request_id, const char *title, const char *rationale),
    umi_ai_coding_patch_init_build_value(candidate, patch_id, request_id, title, rationale))

UmiStatus umi_ai_coding_patch_init(UmiAiCodingPatch *patch,
    const char *patch_id, const char *request_id, const char *title, const char *rationale)
{
    UmiStatus status = UmiAiCodingPatchCheckIdle(patch);
    if (status != UMI_STATUS_OK) return status;
    return coding_patch_init_staged(patch, patch_id, request_id, title, rationale);
}


/*
 * Provide the ai coding patch add file operation used by this module and its client
 * applications.
 */
/* Framework now checks complete bounded metadata and revision capacity before publishing this mutation or accepting its policy. Failed additions and approvals preserve the previous complete value. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_patch_add_file(UmiAiCodingPatch *patch,
                                       const char *path,
                                       UmiAiCodingPatchOperation operation,
                                       const char *before_text,
                                       const char *after_text)
{
    UmiAiCodingPatchFile *file;
    size_t before_length;
    size_t after_length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (patch == NULL || patch->state != UMI_AI_CODING_PATCH_DRAFT ||
        !umi_ai_coding_path_is_safe_relative(path) || before_text == NULL ||
        after_text == NULL || operation < UMI_AI_CODING_PATCH_CREATE ||
        operation > UMI_AI_CODING_PATCH_DELETE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((operation == UMI_AI_CODING_PATCH_CREATE && before_text[0] != '\0') ||
        (operation == UMI_AI_CODING_PATCH_DELETE && after_text[0] != '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (patch->file_count >= UMI_AI_CODING_PATCH_FILE_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    before_length = strlen(before_text);
    after_length = strlen(after_text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (before_length >= UMI_AI_CODING_PATCH_CONTENT_CAPACITY ||
        after_length >= UMI_AI_CODING_PATCH_CONTENT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    file = &patch->files[patch->file_count];
    (void)memset(file, 0, sizeof(*file));
    /* Apply this branch only when its contract condition is satisfied. */
    if (!copy_text(file->path, sizeof(file->path), path) ||
        !copy_text(file->before_text, sizeof(file->before_text), before_text) ||
        !copy_text(file->after_text, sizeof(file->after_text), after_text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    file->operation = operation;
    file->before_length = before_length;
    file->after_length = after_length;
    file->before_hash = umi_ai_coding_text_hash(before_text, before_length);
    file->after_hash = umi_ai_coding_text_hash(after_text, after_length);
    file->removed_lines = line_count(before_text, before_length);
    file->added_lines = line_count(after_text, after_length);
    patch->changed_lines += file->removed_lines + file->added_lines;
    ++patch->file_count;
    ++patch->revision;
    return UMI_STATUS_OK;
}

#endif
static UmiStatus patch_add_file_build(UmiAiCodingPatch *candidate,
    const UmiAiCodingPatch *patch, const char *path,
    UmiAiCodingPatchOperation operation, const char *before_text, const char *after_text)
{
    UmiStatus status = UmiAiCodingPatchCheckIntegrity(patch);
    if (status != UMI_STATUS_OK) return status;
    if (patch->state != UMI_AI_CODING_PATCH_DRAFT || path == NULL ||
        before_text == NULL || after_text == NULL ||
        operation < UMI_AI_CODING_PATCH_CREATE || operation > UMI_AI_CODING_PATCH_DELETE)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t path_length = patch_text_length(path, UMI_AI_TEXT_CAPACITY);
    size_t before_length = patch_text_length(before_text, UMI_AI_CODING_PATCH_CONTENT_CAPACITY);
    size_t after_length = patch_text_length(after_text, UMI_AI_CODING_PATCH_CONTENT_CAPACITY);
    if (path_length == UMI_AI_TEXT_CAPACITY ||
        before_length == UMI_AI_CODING_PATCH_CONTENT_CAPACITY ||
        after_length == UMI_AI_CODING_PATCH_CONTENT_CAPACITY ||
        patch->file_count == UMI_AI_CODING_PATCH_FILE_MAX || patch->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!umi_ai_coding_path_is_safe_relative(path) ||
        (operation == UMI_AI_CODING_PATCH_CREATE && before_length != 0U) ||
        (operation == UMI_AI_CODING_PATCH_DELETE && after_length != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < patch->file_count; ++i)
        if (strcmp(patch->files[i].path, path) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Publication follows all source reads. Inputs may borrow fields in the
     * live patch, including the previously unused destination slot. */
    memcpy(candidate, patch, sizeof *candidate);
    UmiAiCodingPatchFile *file = &candidate->files[candidate->file_count];
    memset(file, 0, sizeof *file);
    memcpy(file->path, path, path_length + 1U);
    memcpy(file->before_text, before_text, before_length + 1U);
    memcpy(file->after_text, after_text, after_length + 1U);
    file->operation = operation;
    file->before_length = before_length; file->after_length = after_length;
    file->before_hash = umi_ai_coding_text_hash(before_text, before_length);
    file->after_hash = umi_ai_coding_text_hash(after_text, after_length);
    file->removed_lines = line_count(before_text, before_length);
    file->added_lines = line_count(after_text, after_length);
    candidate->changed_lines += file->removed_lines + file->added_lines;
    ++candidate->file_count; ++candidate->revision;
    return UMI_STATUS_OK;
}

/* Addition shares the callback transaction guard so a host callback cannot change an outer patch file list. Staged candidate construction remains authoritative. The former implementation is retained for owner review. */
#if 0
UMI_DEFINE_STAGED_VALUE_INIT(umi_ai_coding_patch_add_file, UmiAiCodingPatch, patch,
    (UmiAiCodingPatch *patch, const char *path, UmiAiCodingPatchOperation operation,
     const char *before_text, const char *after_text),
    patch_add_file_build(candidate, patch, path, operation, before_text, after_text))
#endif
static UMI_DEFINE_STAGED_VALUE_INIT(coding_patch_add_file_staged, UmiAiCodingPatch, patch,
    (UmiAiCodingPatch *patch, const char *path, UmiAiCodingPatchOperation operation,
     const char *before_text, const char *after_text),
    patch_add_file_build(candidate, patch, path, operation, before_text, after_text))

UmiStatus umi_ai_coding_patch_add_file(UmiAiCodingPatch *patch, const char *path,
    UmiAiCodingPatchOperation operation, const char *before_text, const char *after_text)
{
    UmiStatus status = UmiAiCodingPatchCheckIdle(patch);
    if (status != UMI_STATUS_OK) return status;
    return coding_patch_add_file_staged(patch, path, operation, before_text, after_text);
}

/* Check that ai coding patch satisfies its contract before another service relies on it. */
/* Framework now checks complete bounded metadata and revision capacity before publishing this mutation or accepting its policy. Failed additions and approvals preserve the previous complete value. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_patch_validate(
    const UmiAiCodingPatch *patch,
    const UmiAiCodingPatchPolicy *policy)
{
    size_t index;
    uint32_t changed_lines = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (patch == NULL || policy == NULL ||
        patch->structure_size < sizeof(UmiAiCodingPatch) ||
        patch->abi_version != UMI_AI_CODING_ABI_VERSION ||
        patch->patch_id[0] == '\0' || patch->request_id[0] == '\0' ||
        patch->file_count == 0U || patch->file_count > policy->maximum_files ||
        patch->changed_lines > policy->maximum_changed_lines) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < patch->file_count; ++index) {
        const UmiAiCodingPatchFile *file = &patch->files[index];
        size_t duplicate;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_ai_coding_path_is_safe_relative(file->path)) {
            return UMI_STATUS_PERMISSION_DENIED;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (file->before_length >= sizeof(file->before_text) ||
            file->after_length >= sizeof(file->after_text) ||
            file->before_text[file->before_length] != '\0' ||
            file->after_text[file->after_length] != '\0' ||
            strlen(file->before_text) != file->before_length ||
            strlen(file->after_text) != file->after_length ||
            umi_ai_coding_text_hash(file->before_text, file->before_length) !=
                file->before_hash ||
            umi_ai_coding_text_hash(file->after_text, file->after_length) !=
                file->after_hash ||
            line_count(file->before_text, file->before_length) !=
                file->removed_lines ||
            line_count(file->after_text, file->after_length) !=
                file->added_lines) {
            return UMI_STATUS_INVALID_STATE;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((file->operation == UMI_AI_CODING_PATCH_CREATE &&
             file->before_length != 0U) ||
            (file->operation == UMI_AI_CODING_PATCH_DELETE &&
             file->after_length != 0U)) {
            return UMI_STATUS_INVALID_STATE;
        }
        /* Visit each bounded item once so every record receives the same rule. */
        for (duplicate = index + 1U; duplicate < patch->file_count; ++duplicate) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(file->path, patch->files[duplicate].path) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
        changed_lines += file->removed_lines + file->added_lines;
        /* Apply this branch only when its contract condition is satisfied. */
        if (file->operation == UMI_AI_CODING_PATCH_CREATE &&
            !policy->allow_create) return UMI_STATUS_PERMISSION_DENIED;
        /* Apply this branch only when its contract condition is satisfied. */
        if (file->operation == UMI_AI_CODING_PATCH_DELETE &&
            !policy->allow_delete) return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (changed_lines != patch->changed_lines) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_patch_validate(
    const UmiAiCodingPatch *patch, const UmiAiCodingPatchPolicy *policy)
{
    UmiStatus status = UmiAiCodingPatchPolicyValidate(policy);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiCodingPatchCheckIntegrity(patch);
    if (status != UMI_STATUS_OK) return status;
    if (patch->file_count == 0U || patch->file_count > policy->maximum_files ||
        patch->changed_lines > policy->maximum_changed_lines) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < patch->file_count; ++i) {
        if ((patch->files[i].operation == UMI_AI_CODING_PATCH_CREATE && !policy->allow_create) ||
            (patch->files[i].operation == UMI_AI_CODING_PATCH_DELETE && !policy->allow_delete))
            return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding patch approve operation used by this module and its client
 * applications.
 */
/* Framework now checks complete bounded metadata and revision capacity before publishing this mutation or accepting its policy. Failed additions and approvals preserve the previous complete value. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_patch_approve(UmiAiCodingPatch *patch,
                                      const char *approved_by)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (patch == NULL || approved_by == NULL || approved_by[0] == '\0' ||
        patch->state != UMI_AI_CODING_PATCH_DRAFT) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!copy_text(patch->approved_by, sizeof(patch->approved_by), approved_by)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    patch->state = UMI_AI_CODING_PATCH_APPROVED;
    ++patch->revision;
    return UMI_STATUS_OK;
}

#endif
/* Approval and rejection must not alter an outer callback transaction; existing integrity and revision admission remain in effect. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_patch_approve(UmiAiCodingPatch *patch, const char *approved_by)
{
    if (patch == NULL || approved_by == NULL || approved_by[0] == '\0' ||
        patch->state != UMI_AI_CODING_PATCH_DRAFT) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiAiCodingPatchCheckIntegrity(patch);
    if (status != UMI_STATUS_OK) return status;
    size_t length = patch_text_length(approved_by, sizeof patch->approved_by);
    if (length == sizeof patch->approved_by || patch->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* No fallible operation follows publication. memmove permits borrowing
     * from existing patch fields, including approved_by itself. */
    memmove(patch->approved_by, approved_by, length + 1U);
    patch->state = UMI_AI_CODING_PATCH_APPROVED; ++patch->revision;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_patch_approve(UmiAiCodingPatch *patch, const char *approved_by)
{
    if (patch != NULL) {
        UmiStatus idle = UmiAiCodingPatchCheckIdle(patch);
        if (idle != UMI_STATUS_OK) return idle;
    }
    if (patch == NULL || approved_by == NULL || approved_by[0] == '\0' ||
        patch->state != UMI_AI_CODING_PATCH_DRAFT) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiAiCodingPatchCheckIntegrity(patch);
    if (status != UMI_STATUS_OK) return status;
    size_t length = patch_text_length(approved_by, sizeof patch->approved_by);
    if (length == sizeof patch->approved_by || patch->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* No fallible operation follows publication. memmove permits borrowing
     * from existing patch fields, including approved_by itself. */
    memmove(patch->approved_by, approved_by, length + 1U);
    patch->state = UMI_AI_CODING_PATCH_APPROVED; ++patch->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding patch reject operation used by this module and its client
 * applications.
 */
/* Framework now checks complete bounded metadata and revision capacity before publishing this mutation or accepting its policy. Failed additions and approvals preserve the previous complete value. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_patch_reject(UmiAiCodingPatch *patch)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (patch == NULL || (patch->state != UMI_AI_CODING_PATCH_DRAFT &&
                          patch->state != UMI_AI_CODING_PATCH_APPROVED)) {
        return UMI_STATUS_INVALID_STATE;
    }
    patch->state = UMI_AI_CODING_PATCH_REJECTED;
    ++patch->revision;
    return UMI_STATUS_OK;
}

#endif
/* Approval and rejection must not alter an outer callback transaction; existing integrity and revision admission remain in effect. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_patch_reject(UmiAiCodingPatch *patch)
{
    if (patch == NULL || (patch->state != UMI_AI_CODING_PATCH_DRAFT &&
        patch->state != UMI_AI_CODING_PATCH_APPROVED)) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiAiCodingPatchCheckIntegrity(patch);
    if (status != UMI_STATUS_OK) return status;
    if (patch->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    patch->state = UMI_AI_CODING_PATCH_REJECTED; ++patch->revision;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_patch_reject(UmiAiCodingPatch *patch)
{
    if (patch != NULL) {
        UmiStatus idle = UmiAiCodingPatchCheckIdle(patch);
        if (idle != UMI_STATUS_OK) return idle;
    }
    if (patch == NULL || (patch->state != UMI_AI_CODING_PATCH_DRAFT &&
        patch->state != UMI_AI_CODING_PATCH_APPROVED)) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiAiCodingPatchCheckIntegrity(patch);
    if (status != UMI_STATUS_OK) return status;
    if (patch->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    patch->state = UMI_AI_CODING_PATCH_REJECTED; ++patch->revision;
    return UMI_STATUS_OK;
}

/* Check that adapter satisfies its contract before another service relies on it. */
static int adapter_valid(const UmiAiCodingFileAdapter *adapter)
{
    return adapter != NULL && adapter->structure_size >= sizeof(*adapter) &&
           adapter->abi_version == UMI_AI_CODING_ABI_VERSION &&
           adapter->read != NULL && adapter->write != NULL &&
           adapter->remove != NULL;
}

/* Hosts that snapshot callback slots must admit the ABI before copying the
 * complete adapter. This check performs no callback or filesystem operation. */
UmiStatus UmiAiCodingFileAdapterValidate(const UmiAiCodingFileAdapter *adapter)
{
    return adapter_valid(adapter) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* Provide the verify before operation used by this module and its client applications. */
/* An OK callback status does not validate its reported byte count. Refuse oversized results before hashing or allowing writes. The former implementation is retained for owner review. */
#if 0
static UmiStatus verify_before(const UmiAiCodingPatchFile *file,
                               const UmiAiCodingFileAdapter *adapter)
{
    char current[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t length = 0U;
    UmiStatus status = adapter->read(
        adapter->user_data, file->path, current, sizeof(current), &length);
    /* Apply this branch only when its contract condition is satisfied. */
    if (file->operation == UMI_AI_CODING_PATCH_CREATE) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
        return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status != UMI_STATUS_OK || length >= sizeof(current)) return status;
    return umi_ai_coding_text_hash(current, length) == file->before_hash
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

#endif
static UmiStatus verify_before(const UmiAiCodingPatchFile *file,
                               const UmiAiCodingFileAdapter *adapter)
{
    char current[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t length = 0U;
    UmiStatus status = adapter->read(
        adapter->user_data, file->path, current, sizeof(current), &length);
    /* Apply this branch only when its contract condition is satisfied. */
    if (file->operation == UMI_AI_CODING_PATCH_CREATE) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
        return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status != UMI_STATUS_OK) return status;
    if (length >= sizeof(current)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_ai_coding_text_hash(current, length) == file->before_hash
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/* Provide the apply file operation used by this module and its client applications. */
static UmiStatus apply_file(const UmiAiCodingPatchFile *file,
                            const UmiAiCodingFileAdapter *adapter)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (file->operation == UMI_AI_CODING_PATCH_DELETE) {
        return adapter->remove(adapter->user_data, file->path);
    }
    return adapter->write(adapter->user_data, file->path, file->after_text,
                          file->after_length);
}

/* Provide the rollback file operation used by this module and its client applications. */
/* Compensation callback statuses now reach the detailed transaction report; a restoration attempt is not silently treated as a successful repair. The former implementation is retained for owner review. */
#if 0
static void rollback_file(const UmiAiCodingPatchFile *file,
                          const UmiAiCodingFileAdapter *adapter)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (file->operation == UMI_AI_CODING_PATCH_CREATE) {
        (void)adapter->remove(adapter->user_data, file->path);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)adapter->write(adapter->user_data, file->path, file->before_text,
                             file->before_length);
    }
}

#endif
static UmiStatus rollback_file(const UmiAiCodingPatchFile *file,
                          const UmiAiCodingFileAdapter *adapter)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (file->operation == UMI_AI_CODING_PATCH_CREATE) {
        return adapter->remove(adapter->user_data, file->path);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return adapter->write(adapter->user_data, file->path, file->before_text,
                             file->before_length);
    }
}

/*
 * Perform ai coding patch through the module contract so client applications do not
 * duplicate its policy.
 */
/* Reserve a revision before external callbacks. Existing rollback attempts remain unchanged and do not prove restoration. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_patch_apply(UmiAiCodingPatch *patch,
                                    const UmiAiCodingPatchPolicy *policy,
                                    const UmiAiCodingFileAdapter *adapter)
{
    size_t index;
    UmiStatus status = umi_ai_coding_patch_validate(patch, policy);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!adapter_valid(adapter)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((policy->require_approval &&
         patch->state != UMI_AI_CODING_PATCH_APPROVED) ||
        (!policy->require_approval &&
         patch->state != UMI_AI_CODING_PATCH_DRAFT &&
         patch->state != UMI_AI_CODING_PATCH_APPROVED)) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < patch->file_count; ++index) {
        status = verify_before(&patch->files[index], adapter);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < patch->file_count; ++index) {
        status = apply_file(&patch->files[index], adapter);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            /* A callback may report an error after a partial write.  Restore
             * the current file as well as every earlier completed file. */
            rollback_file(&patch->files[index], adapter);
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
            while (index > 0U) rollback_file(&patch->files[--index], adapter);
            return status;
        }
    }
    patch->state = UMI_AI_CODING_PATCH_APPLIED;
    ++patch->revision;
    return UMI_STATUS_OK;
}

#endif
/* Detailed transaction execution now preserves the primary result, records compensation and checks restoration while enforcing callback ownership. The compatibility interface returns the original primary status. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_patch_apply(UmiAiCodingPatch *patch,
                                    const UmiAiCodingPatchPolicy *policy,
                                    const UmiAiCodingFileAdapter *adapter)
{
    size_t index;
    UmiStatus status = umi_ai_coding_patch_validate(patch, policy);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!adapter_valid(adapter)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((policy->require_approval &&
         patch->state != UMI_AI_CODING_PATCH_APPROVED) ||
        (!policy->require_approval &&
         patch->state != UMI_AI_CODING_PATCH_DRAFT &&
         patch->state != UMI_AI_CODING_PATCH_APPROVED)) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Reserve the state transition before invoking any host callback. */
    if (patch->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < patch->file_count; ++index) {
        status = verify_before(&patch->files[index], adapter);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < patch->file_count; ++index) {
        status = apply_file(&patch->files[index], adapter);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            /* A callback may report an error after a partial write.  Restore
             * the current file as well as every earlier completed file. */
            rollback_file(&patch->files[index], adapter);
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
            while (index > 0U) rollback_file(&patch->files[--index], adapter);
            return status;
        }
    }
    patch->state = UMI_AI_CODING_PATCH_APPLIED;
    ++patch->revision;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_patch_apply(UmiAiCodingPatch *patch,
    const UmiAiCodingPatchPolicy *policy, const UmiAiCodingFileAdapter *adapter)
{
    UmiAiCodingPatchTransactionReport report;
    return UmiAiCodingPatchApplyWithReport(patch, policy, adapter, &report);
}

/* Provide the verify after operation used by this module and its client applications. */
/* An OK callback status does not validate its reported byte count. Refuse oversized results before hashing or allowing writes. The former implementation is retained for owner review. */
#if 0
static UmiStatus verify_after(const UmiAiCodingPatchFile *file,
                              const UmiAiCodingFileAdapter *adapter)
{
    char current[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t length = 0U;
    UmiStatus status = adapter->read(
        adapter->user_data, file->path, current, sizeof(current), &length);
    /* Apply this branch only when its contract condition is satisfied. */
    if (file->operation == UMI_AI_CODING_PATCH_DELETE) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
        return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status != UMI_STATUS_OK || length >= sizeof(current)) return status;
    return umi_ai_coding_text_hash(current, length) == file->after_hash
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

#endif
static UmiStatus verify_after(const UmiAiCodingPatchFile *file,
                              const UmiAiCodingFileAdapter *adapter)
{
    char current[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t length = 0U;
    UmiStatus status = adapter->read(
        adapter->user_data, file->path, current, sizeof(current), &length);
    /* Apply this branch only when its contract condition is satisfied. */
    if (file->operation == UMI_AI_CODING_PATCH_DELETE) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
        return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status != UMI_STATUS_OK) return status;
    if (length >= sizeof(current)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_ai_coding_text_hash(current, length) == file->after_hash
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/*
 * Provide the ai coding patch revert operation used by this module and its client
 * applications.
 */
/* Reversion admits physical array bounds and complete metadata, and reserves the final revision before callbacks; it does not reapply a forward deletion policy. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_patch_revert(UmiAiCodingPatch *patch,
                                     const UmiAiCodingFileAdapter *adapter)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (patch == NULL || patch->state != UMI_AI_CODING_PATCH_APPLIED) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!adapter_valid(adapter)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < patch->file_count; ++index) {
        status = verify_after(&patch->files[index], adapter);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = patch->file_count; index > 0U; --index) {
        const UmiAiCodingPatchFile *file = &patch->files[index - 1U];
        /* Apply this branch only when its contract condition is satisfied. */
        if (file->operation == UMI_AI_CODING_PATCH_CREATE) {
            status = adapter->remove(adapter->user_data, file->path);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            status = adapter->write(adapter->user_data, file->path,
                                    file->before_text, file->before_length);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            size_t restore;
            /* Keep the ledger truthful if a revert callback fails midway: put
             * this and already reverted files back into their applied form. */
            (void)apply_file(file, adapter);
            /* Visit each bounded item once so every record receives the same rule. */
            for (restore = index; restore < patch->file_count; ++restore) {
                (void)apply_file(&patch->files[restore], adapter);
            }
            return status;
        }
    }
    patch->state = UMI_AI_CODING_PATCH_REVERTED;
    ++patch->revision;
    return UMI_STATUS_OK;
}

#endif
/* Detailed transaction execution now preserves the primary result, records compensation and checks restoration while enforcing callback ownership. The compatibility interface returns the original primary status. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_patch_revert(UmiAiCodingPatch *patch,
                                     const UmiAiCodingFileAdapter *adapter)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (patch == NULL || patch->state != UMI_AI_CODING_PATCH_APPLIED) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = UmiAiCodingPatchCheckIntegrity(patch);
    if (status != UMI_STATUS_OK) return status;
    if (patch->file_count == 0U) return UMI_STATUS_INVALID_STATE;
    if (patch->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!adapter_valid(adapter)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < patch->file_count; ++index) {
        status = verify_after(&patch->files[index], adapter);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = patch->file_count; index > 0U; --index) {
        const UmiAiCodingPatchFile *file = &patch->files[index - 1U];
        /* Apply this branch only when its contract condition is satisfied. */
        if (file->operation == UMI_AI_CODING_PATCH_CREATE) {
            status = adapter->remove(adapter->user_data, file->path);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            status = adapter->write(adapter->user_data, file->path,
                                    file->before_text, file->before_length);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            size_t restore;
            /* Keep the ledger truthful if a revert callback fails midway: put
             * this and already reverted files back into their applied form. */
            (void)apply_file(file, adapter);
            /* Visit each bounded item once so every record receives the same rule. */
            for (restore = index; restore < patch->file_count; ++restore) {
                (void)apply_file(&patch->files[restore], adapter);
            }
            return status;
        }
    }
    patch->state = UMI_AI_CODING_PATCH_REVERTED;
    ++patch->revision;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_patch_revert(UmiAiCodingPatch *patch,
    const UmiAiCodingFileAdapter *adapter)
{
    UmiAiCodingPatchTransactionReport report;
    return UmiAiCodingPatchRevertWithReport(patch, adapter, &report);
}

/* All callback slots are captured after ABI admission. Their borrowed user
 * data and the patch must remain live until normal callback return. */
static UmiStatus patch_execute_transaction(UmiAiCodingPatch *patch,
    const UmiAiCodingPatchPolicy *policy, const UmiAiCodingFileAdapter *adapter,
    int reverting, UmiAiCodingPatchTransactionReport *outReport)
{
    if (patch == NULL) return reverting ? UMI_STATUS_INVALID_STATE : UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingPatchCheckIdle(patch);
    if (status != UMI_STATUS_OK) return status;
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (reverting) {
        if (patch->state != UMI_AI_CODING_PATCH_APPLIED) return UMI_STATUS_INVALID_STATE;
        status = UmiAiCodingPatchCheckIntegrity(patch);
        if (status != UMI_STATUS_OK) return status;
        if (patch->file_count == 0U) return UMI_STATUS_INVALID_STATE;
        if (patch->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (!adapter_valid(adapter)) return UMI_STATUS_INVALID_ARGUMENT;
    } else {
        status = umi_ai_coding_patch_validate(patch, policy);
        if (status != UMI_STATUS_OK) return status;
        if (!adapter_valid(adapter)) return UMI_STATUS_INVALID_ARGUMENT;
        if ((policy->require_approval && patch->state != UMI_AI_CODING_PATCH_APPROVED) ||
            (!policy->require_approval && patch->state != UMI_AI_CODING_PATCH_DRAFT &&
             patch->state != UMI_AI_CODING_PATCH_APPROVED)) return UMI_STATUS_PERMISSION_DENIED;
        if (patch->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiAiCodingFileAdapter captured = *adapter;
    UmiAiCodingPatchTransactionReport report = {0};
    report.admitted = 1;
    report.primary_status = UMI_STATUS_OK;
    report.compensation_status = UMI_STATUS_OK;
    report.failed_file_index = SIZE_MAX;
    report.revision_before = patch->revision;
    CodingPatchTransactionFrame frame = {patch, codingPatchTransactions};
    codingPatchTransactions = &frame;
    for (size_t i = 0U; i < patch->file_count; ++i) {
        status = reverting ? verify_after(&patch->files[i], &captured) : verify_before(&patch->files[i], &captured);
        if (status != UMI_STATUS_OK) {
            report.primary_status = status; report.failed_file_index = i;
            goto finished;
        }
        ++report.verified_files;
    }
    for (size_t step = 0U; step < patch->file_count; ++step) {
        size_t index = reverting ? patch->file_count - 1U - step : step;
        ++report.mutation_attempts;
        status = reverting ? rollback_file(&patch->files[index], &captured) : apply_file(&patch->files[index], &captured);
        if (status == UMI_STATUS_OK) { ++report.mutations_completed; continue; }
        report.primary_status = status;
        report.failed_file_index = index;
        /* The failing callback may already have changed its file. Compensate
         * that file too, then every completed mutation in reverse order. */
        for (size_t n = step + 1U; n > 0U; --n) {
            size_t restore = reverting ? patch->file_count - n : n - 1U;
            ++report.compensation_attempts;
            UmiStatus restored = reverting ? apply_file(&patch->files[restore], &captured) : rollback_file(&patch->files[restore], &captured);
            if (restored != UMI_STATUS_OK) {
                ++report.compensation_failures;
                if (report.compensation_status == UMI_STATUS_OK) report.compensation_status = restored;
            }
        }
        /* Readback evidence is distinct from callback status. Continue all
         * checks even after one failure so partial recovery stays visible. */
        for (size_t n = 0U; n <= step; ++n) {
            size_t restore = reverting ? patch->file_count - 1U - n : n;
            ++report.restoration_checks;
            UmiStatus checked = reverting ? verify_after(&patch->files[restore], &captured) : verify_before(&patch->files[restore], &captured);
            if (checked != UMI_STATUS_OK) {
                ++report.restoration_failures;
                if (report.compensation_status == UMI_STATUS_OK) report.compensation_status = checked;
            }
        }
        goto finished;
    }
    patch->state = reverting ? UMI_AI_CODING_PATCH_REVERTED : UMI_AI_CODING_PATCH_APPLIED;
finished:
    /* An attempted write is observable even when compensation succeeds. The
     * state remains the last committed state on failure, not a filesystem claim. */
    if (report.mutation_attempts != 0U) ++patch->revision;
    report.revision_after = patch->revision;
    codingPatchTransactions = frame.previous;
    *outReport = report;
    return report.primary_status;
}

UmiStatus UmiAiCodingPatchApplyWithReport(UmiAiCodingPatch *patch,
    const UmiAiCodingPatchPolicy *policy, const UmiAiCodingFileAdapter *adapter,
    UmiAiCodingPatchTransactionReport *outReport)
{
    return patch_execute_transaction(patch, policy, adapter, 0, outReport);
}

UmiStatus UmiAiCodingPatchRevertWithReport(UmiAiCodingPatch *patch,
    const UmiAiCodingFileAdapter *adapter, UmiAiCodingPatchTransactionReport *outReport)
{
    return patch_execute_transaction(patch, NULL, adapter, 1, outReport);
}
