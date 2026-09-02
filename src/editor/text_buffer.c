/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/text_buffer.c
 *
 * PURPOSE:
 *   Implement mutable editor storage with overflow-safe growth and half-open
 *   replacement ranges. The storage is always NUL-terminated for C adapters,
 *   while byte_count remains authoritative and embedded NUL bytes are allowed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/text_buffer.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorTextBuffer {
    char *bytes;
    size_t byte_count;
    size_t capacity;
    uint64_t revision;
    uint64_t save_revision;
};

/* Provide the add overflows operation used by this module and its client applications. */
static int add_overflows(size_t left, size_t right)
{
    return right > SIZE_MAX - left;
}

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t current)
{
    return current == UINT64_MAX ? 1U : current + 1U;
}

/*
 * Initialise editor text buffer from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_text_buffer_create(size_t initial_capacity,
                                         UmiEditorTextBuffer **out_buffer)
{
    UmiEditorTextBuffer *buffer;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_buffer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_buffer = NULL;
    /* Apply this branch only when its contract condition is satisfied. */
    if (initial_capacity == 0U) {
        initial_capacity = UMI_EDITOR_TEXT_BUFFER_DEFAULT_CAPACITY;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (initial_capacity == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;

    buffer = (UmiEditorTextBuffer *)calloc(1U, sizeof(*buffer));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    buffer->bytes = (char *)calloc(initial_capacity + 1U, sizeof(char));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer->bytes == NULL) {
        free(buffer);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    buffer->capacity = initial_capacity;
    buffer->revision = 1U;
    buffer->save_revision = 1U;
    *out_buffer = buffer;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor text buffer so the same storage can be reused
 * safely.
 */
void umi_editor_text_buffer_destroy(UmiEditorTextBuffer *buffer)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL) return;
    free(buffer->bytes);
    buffer->bytes = NULL;
    free(buffer);
}

/*
 * Provide the editor text buffer reserve operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_text_buffer_reserve(UmiEditorTextBuffer *buffer,
                                          size_t required_capacity)
{
    size_t new_capacity;
    char *replacement;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required_capacity <= buffer->capacity) return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (required_capacity == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;

    new_capacity = buffer->capacity > 0U
        ? buffer->capacity
        : UMI_EDITOR_TEXT_BUFFER_DEFAULT_CAPACITY;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (new_capacity < required_capacity) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (new_capacity > (SIZE_MAX - 1U) / 2U) {
            new_capacity = required_capacity;
            break;
        }
        new_capacity *= 2U;
    }

    replacement = (char *)realloc(buffer->bytes, new_capacity + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    buffer->bytes = replacement;
    buffer->capacity = new_capacity;
    buffer->bytes[buffer->byte_count] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the editor text buffer replace operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_text_buffer_replace(UmiEditorTextBuffer *buffer,
                                          size_t offset,
                                          size_t removed_byte_count,
                                          const char *inserted_bytes,
                                          size_t inserted_byte_count)
{
    size_t suffix_offset;
    size_t suffix_count;
    size_t new_count;
    char *aliased_copy = NULL;
    const char *insertion = inserted_bytes;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL ||
        (inserted_bytes == NULL && inserted_byte_count > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (offset > buffer->byte_count ||
        removed_byte_count > buffer->byte_count - offset) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (add_overflows(buffer->byte_count - removed_byte_count,
                      inserted_byte_count)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    new_count = buffer->byte_count - removed_byte_count + inserted_byte_count;
    /* Apply this branch only when its contract condition is satisfied. */
    if (inserted_byte_count > 0U) {
        uintptr_t source_address = (uintptr_t)(const void *)inserted_bytes;
        uintptr_t buffer_begin = (uintptr_t)(const void *)buffer->bytes;
        uintptr_t buffer_end = buffer_begin + buffer->byte_count;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (source_address >= buffer_begin && source_address < buffer_end) {
            aliased_copy = (char *)malloc(inserted_byte_count);
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (aliased_copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
            (void)memcpy(aliased_copy, inserted_bytes, inserted_byte_count);
            insertion = aliased_copy;
        }
    }
    status = umi_editor_text_buffer_reserve(buffer, new_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(aliased_copy);
        return status;
    }

    suffix_offset = offset + removed_byte_count;
    suffix_count = buffer->byte_count - suffix_offset;
    /* Apply this branch only when its contract condition is satisfied. */
    if (inserted_byte_count != removed_byte_count) {
        (void)memmove(buffer->bytes + offset + inserted_byte_count,
                      buffer->bytes + suffix_offset,
                      suffix_count);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (inserted_byte_count > 0U) {
        /* memmove permits insertion from a slice of the same buffer. */
        (void)memmove(buffer->bytes + offset,
                      insertion,
                      inserted_byte_count);
    }
    buffer->byte_count = new_count;
    buffer->bytes[new_count] = '\0';
    buffer->revision = next_revision(buffer->revision);
    free(aliased_copy);
    return UMI_STATUS_OK;
}

/*
 * Copy editor text buffer into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_editor_text_buffer_set(UmiEditorTextBuffer *buffer,
                                      const char *bytes,
                                      size_t byte_count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL || (bytes == NULL && byte_count > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_editor_text_buffer_replace(buffer,
                                           0U,
                                           buffer->byte_count,
                                           bytes,
                                           byte_count);
}

/* Add editor text buffer only after its inputs and available capacity have been checked. */
UmiStatus umi_editor_text_buffer_insert(UmiEditorTextBuffer *buffer,
                                         size_t offset,
                                         const char *bytes,
                                         size_t byte_count)
{
    return umi_editor_text_buffer_replace(buffer, offset, 0U, bytes, byte_count);
}

/*
 * Remove editor text buffer while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_text_buffer_erase(UmiEditorTextBuffer *buffer,
                                        size_t offset,
                                        size_t byte_count)
{
    return umi_editor_text_buffer_replace(buffer, offset, byte_count, NULL, 0U);
}

/*
 * Copy editor text buffer into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_editor_text_buffer_copy(const UmiEditorTextBuffer *buffer,
                                       size_t offset,
                                       size_t byte_count,
                                       char *out_bytes,
                                       size_t out_capacity,
                                       size_t *out_byte_count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL || out_bytes == NULL || out_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (offset > buffer->byte_count || byte_count > buffer->byte_count - offset) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (byte_count + 1U > out_capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (byte_count > 0U) {
        (void)memcpy(out_bytes, buffer->bytes + offset, byte_count);
    }
    out_bytes[byte_count] = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_byte_count != NULL) *out_byte_count = byte_count;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor text buffer view operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_text_buffer_view(const UmiEditorTextBuffer *buffer,
                                       UmiEditorTextBufferView *out_view)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL || out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    out_view->struct_size = (uint32_t)sizeof(*out_view);
    out_view->api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    out_view->bytes = buffer->bytes;
    out_view->byte_count = buffer->byte_count;
    out_view->capacity = buffer->capacity;
    out_view->revision = buffer->revision;
    out_view->save_revision = buffer->save_revision;
    out_view->dirty = buffer->revision != buffer->save_revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor text buffer mark saved operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_text_buffer_mark_saved(UmiEditorTextBuffer *buffer)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    buffer->save_revision = buffer->revision;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor text buffer without changing their
 * state.
 */
size_t umi_editor_text_buffer_size(const UmiEditorTextBuffer *buffer)
{
    return buffer != NULL ? buffer->byte_count : 0U;
}

/*
 * Provide the editor text buffer revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_text_buffer_revision(const UmiEditorTextBuffer *buffer)
{
    return buffer != NULL ? buffer->revision : 0U;
}

/*
 * Provide the editor text buffer is dirty operation used by this module and its client
 * applications.
 */
int umi_editor_text_buffer_is_dirty(const UmiEditorTextBuffer *buffer)
{
    return buffer != NULL && buffer->revision != buffer->save_revision;
}
