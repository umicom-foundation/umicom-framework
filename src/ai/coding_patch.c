/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/coding_patch.c
 *
 * PURPOSE:
 *   Implement bounded patch previews, approval, conflict detection, controlled
 *   application, failure rollback and later user-requested reversion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Applying a patch is a small transaction.  Every current file is checked
 * against the reviewed before-hash before the first write, and completed writes
 * are rolled back if a later callback fails.
 */
#include "umicom/ai/coding_patch.h"

#include <stdio.h>
#include <string.h>

static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL) return 0;
    written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

static uint32_t line_count(const char *text, size_t length)
{
    uint32_t lines = length > 0U ? 1U : 0U;
    size_t index;
    for (index = 0U; index < length; ++index) {
        if (text[index] == '\n' && index + 1U < length) ++lines;
    }
    return lines;
}

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

UmiStatus umi_ai_coding_patch_init(UmiAiCodingPatch *patch,
                                   const char *patch_id,
                                   const char *request_id,
                                   const char *title,
                                   const char *rationale)
{
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
    if (!copy_text(patch->patch_id, sizeof(patch->patch_id), patch_id) ||
        !copy_text(patch->request_id, sizeof(patch->request_id), request_id) ||
        !copy_text(patch->title, sizeof(patch->title), title) ||
        !copy_text(patch->rationale, sizeof(patch->rationale), rationale)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_patch_add_file(UmiAiCodingPatch *patch,
                                       const char *path,
                                       UmiAiCodingPatchOperation operation,
                                       const char *before_text,
                                       const char *after_text)
{
    UmiAiCodingPatchFile *file;
    size_t before_length;
    size_t after_length;
    if (patch == NULL || patch->state != UMI_AI_CODING_PATCH_DRAFT ||
        !umi_ai_coding_path_is_safe_relative(path) || before_text == NULL ||
        after_text == NULL || operation < UMI_AI_CODING_PATCH_CREATE ||
        operation > UMI_AI_CODING_PATCH_DELETE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((operation == UMI_AI_CODING_PATCH_CREATE && before_text[0] != '\0') ||
        (operation == UMI_AI_CODING_PATCH_DELETE && after_text[0] != '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (patch->file_count >= UMI_AI_CODING_PATCH_FILE_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    before_length = strlen(before_text);
    after_length = strlen(after_text);
    if (before_length >= UMI_AI_CODING_PATCH_CONTENT_CAPACITY ||
        after_length >= UMI_AI_CODING_PATCH_CONTENT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    file = &patch->files[patch->file_count];
    (void)memset(file, 0, sizeof(*file));
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

UmiStatus umi_ai_coding_patch_validate(
    const UmiAiCodingPatch *patch,
    const UmiAiCodingPatchPolicy *policy)
{
    size_t index;
    uint32_t changed_lines = 0U;
    if (patch == NULL || policy == NULL ||
        patch->structure_size < sizeof(UmiAiCodingPatch) ||
        patch->abi_version != UMI_AI_CODING_ABI_VERSION ||
        patch->patch_id[0] == '\0' || patch->request_id[0] == '\0' ||
        patch->file_count == 0U || patch->file_count > policy->maximum_files ||
        patch->changed_lines > policy->maximum_changed_lines) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < patch->file_count; ++index) {
        const UmiAiCodingPatchFile *file = &patch->files[index];
        size_t duplicate;
        if (!umi_ai_coding_path_is_safe_relative(file->path)) {
            return UMI_STATUS_PERMISSION_DENIED;
        }
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
        if ((file->operation == UMI_AI_CODING_PATCH_CREATE &&
             file->before_length != 0U) ||
            (file->operation == UMI_AI_CODING_PATCH_DELETE &&
             file->after_length != 0U)) {
            return UMI_STATUS_INVALID_STATE;
        }
        for (duplicate = index + 1U; duplicate < patch->file_count; ++duplicate) {
            if (strcmp(file->path, patch->files[duplicate].path) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
        changed_lines += file->removed_lines + file->added_lines;
        if (file->operation == UMI_AI_CODING_PATCH_CREATE &&
            !policy->allow_create) return UMI_STATUS_PERMISSION_DENIED;
        if (file->operation == UMI_AI_CODING_PATCH_DELETE &&
            !policy->allow_delete) return UMI_STATUS_PERMISSION_DENIED;
    }
    if (changed_lines != patch->changed_lines) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_patch_approve(UmiAiCodingPatch *patch,
                                      const char *approved_by)
{
    if (patch == NULL || approved_by == NULL || approved_by[0] == '\0' ||
        patch->state != UMI_AI_CODING_PATCH_DRAFT) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (!copy_text(patch->approved_by, sizeof(patch->approved_by), approved_by)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    patch->state = UMI_AI_CODING_PATCH_APPROVED;
    ++patch->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_patch_reject(UmiAiCodingPatch *patch)
{
    if (patch == NULL || (patch->state != UMI_AI_CODING_PATCH_DRAFT &&
                          patch->state != UMI_AI_CODING_PATCH_APPROVED)) {
        return UMI_STATUS_INVALID_STATE;
    }
    patch->state = UMI_AI_CODING_PATCH_REJECTED;
    ++patch->revision;
    return UMI_STATUS_OK;
}

static int adapter_valid(const UmiAiCodingFileAdapter *adapter)
{
    return adapter != NULL && adapter->structure_size >= sizeof(*adapter) &&
           adapter->abi_version == UMI_AI_CODING_ABI_VERSION &&
           adapter->read != NULL && adapter->write != NULL &&
           adapter->remove != NULL;
}

static UmiStatus verify_before(const UmiAiCodingPatchFile *file,
                               const UmiAiCodingFileAdapter *adapter)
{
    char current[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t length = 0U;
    UmiStatus status = adapter->read(
        adapter->user_data, file->path, current, sizeof(current), &length);
    if (file->operation == UMI_AI_CODING_PATCH_CREATE) {
        if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
        return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
    }
    if (status != UMI_STATUS_OK || length >= sizeof(current)) return status;
    return umi_ai_coding_text_hash(current, length) == file->before_hash
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

static UmiStatus apply_file(const UmiAiCodingPatchFile *file,
                            const UmiAiCodingFileAdapter *adapter)
{
    if (file->operation == UMI_AI_CODING_PATCH_DELETE) {
        return adapter->remove(adapter->user_data, file->path);
    }
    return adapter->write(adapter->user_data, file->path, file->after_text,
                          file->after_length);
}

static void rollback_file(const UmiAiCodingPatchFile *file,
                          const UmiAiCodingFileAdapter *adapter)
{
    if (file->operation == UMI_AI_CODING_PATCH_CREATE) {
        (void)adapter->remove(adapter->user_data, file->path);
    } else {
        (void)adapter->write(adapter->user_data, file->path, file->before_text,
                             file->before_length);
    }
}

UmiStatus umi_ai_coding_patch_apply(UmiAiCodingPatch *patch,
                                    const UmiAiCodingPatchPolicy *policy,
                                    const UmiAiCodingFileAdapter *adapter)
{
    size_t index;
    UmiStatus status = umi_ai_coding_patch_validate(patch, policy);
    if (status != UMI_STATUS_OK) return status;
    if (!adapter_valid(adapter)) return UMI_STATUS_INVALID_ARGUMENT;
    if ((policy->require_approval &&
         patch->state != UMI_AI_CODING_PATCH_APPROVED) ||
        (!policy->require_approval &&
         patch->state != UMI_AI_CODING_PATCH_DRAFT &&
         patch->state != UMI_AI_CODING_PATCH_APPROVED)) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    for (index = 0U; index < patch->file_count; ++index) {
        status = verify_before(&patch->files[index], adapter);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < patch->file_count; ++index) {
        status = apply_file(&patch->files[index], adapter);
        if (status != UMI_STATUS_OK) {
            /* A callback may report an error after a partial write.  Restore
             * the current file as well as every earlier completed file. */
            rollback_file(&patch->files[index], adapter);
            while (index > 0U) rollback_file(&patch->files[--index], adapter);
            return status;
        }
    }
    patch->state = UMI_AI_CODING_PATCH_APPLIED;
    ++patch->revision;
    return UMI_STATUS_OK;
}

static UmiStatus verify_after(const UmiAiCodingPatchFile *file,
                              const UmiAiCodingFileAdapter *adapter)
{
    char current[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t length = 0U;
    UmiStatus status = adapter->read(
        adapter->user_data, file->path, current, sizeof(current), &length);
    if (file->operation == UMI_AI_CODING_PATCH_DELETE) {
        if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
        return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
    }
    if (status != UMI_STATUS_OK || length >= sizeof(current)) return status;
    return umi_ai_coding_text_hash(current, length) == file->after_hash
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

UmiStatus umi_ai_coding_patch_revert(UmiAiCodingPatch *patch,
                                     const UmiAiCodingFileAdapter *adapter)
{
    size_t index;
    UmiStatus status;
    if (patch == NULL || patch->state != UMI_AI_CODING_PATCH_APPLIED) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (!adapter_valid(adapter)) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < patch->file_count; ++index) {
        status = verify_after(&patch->files[index], adapter);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = patch->file_count; index > 0U; --index) {
        const UmiAiCodingPatchFile *file = &patch->files[index - 1U];
        if (file->operation == UMI_AI_CODING_PATCH_CREATE) {
            status = adapter->remove(adapter->user_data, file->path);
        } else {
            status = adapter->write(adapter->user_data, file->path,
                                    file->before_text, file->before_length);
        }
        if (status != UMI_STATUS_OK) {
            size_t restore;
            /* Keep the ledger truthful if a revert callback fails midway: put
             * this and already reverted files back into their applied form. */
            (void)apply_file(file, adapter);
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
