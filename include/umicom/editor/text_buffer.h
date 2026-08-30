/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/text_buffer.h
 *
 * PURPOSE:
 *   Provide a reusable, toolkit-neutral mutable text buffer with deterministic
 *   revision, save-point and capacity semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_TEXT_BUFFER_H
#define UMICOM_EDITOR_TEXT_BUFFER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_TEXT_BUFFER_API_VERSION 2U
#define UMI_EDITOR_TEXT_BUFFER_DEFAULT_CAPACITY 4096U

typedef struct UmiEditorTextBuffer UmiEditorTextBuffer;

/** A borrowed view remains valid until the next successful mutation. */
typedef struct UmiEditorTextBufferView {
    uint32_t struct_size;
    uint32_t api_version;
    const char *bytes;
    size_t byte_count;
    size_t capacity;
    uint64_t revision;
    uint64_t save_revision;
    int dirty;
} UmiEditorTextBufferView;

UmiStatus umi_editor_text_buffer_create(size_t initial_capacity,
                                         UmiEditorTextBuffer **out_buffer);
void umi_editor_text_buffer_destroy(UmiEditorTextBuffer *buffer);

UmiStatus umi_editor_text_buffer_reserve(UmiEditorTextBuffer *buffer,
                                          size_t required_capacity);
UmiStatus umi_editor_text_buffer_set(UmiEditorTextBuffer *buffer,
                                      const char *bytes,
                                      size_t byte_count);
UmiStatus umi_editor_text_buffer_insert(UmiEditorTextBuffer *buffer,
                                         size_t offset,
                                         const char *bytes,
                                         size_t byte_count);
UmiStatus umi_editor_text_buffer_erase(UmiEditorTextBuffer *buffer,
                                        size_t offset,
                                        size_t byte_count);
UmiStatus umi_editor_text_buffer_replace(UmiEditorTextBuffer *buffer,
                                          size_t offset,
                                          size_t removed_byte_count,
                                          const char *inserted_bytes,
                                          size_t inserted_byte_count);
UmiStatus umi_editor_text_buffer_copy(const UmiEditorTextBuffer *buffer,
                                       size_t offset,
                                       size_t byte_count,
                                       char *out_bytes,
                                       size_t out_capacity,
                                       size_t *out_byte_count);

UmiStatus umi_editor_text_buffer_view(const UmiEditorTextBuffer *buffer,
                                       UmiEditorTextBufferView *out_view);
UmiStatus umi_editor_text_buffer_mark_saved(UmiEditorTextBuffer *buffer);
size_t umi_editor_text_buffer_size(const UmiEditorTextBuffer *buffer);
uint64_t umi_editor_text_buffer_revision(const UmiEditorTextBuffer *buffer);
int umi_editor_text_buffer_is_dirty(const UmiEditorTextBuffer *buffer);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_TEXT_BUFFER_H */
