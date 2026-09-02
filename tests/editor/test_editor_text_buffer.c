/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_text_buffer.c
 *
 * PURPOSE:
 *   Implement the test editor text buffer behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework text buffer tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

#include "umicom/editor/text_buffer.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorTextBuffer *buffer = NULL;
    UmiEditorTextBufferView view;
    char copy[64];
    size_t copied = 0U;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_text_buffer_create(4U, &buffer) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_text_buffer_set(buffer, "alpha\nbeta", 10U) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_text_buffer_insert(buffer, 5U, " framework", 10U) != UMI_STATUS_OK) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_text_buffer_replace(buffer, 16U, 4U, "editor", 6U) != UMI_STATUS_OK) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_text_buffer_view(buffer, &view) != UMI_STATUS_OK) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (view.byte_count != 22U || !view.dirty ||
        memcmp(view.bytes, "alpha framework\neditor", 22U) != 0) return 6;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_text_buffer_copy(buffer, 6U, 9U, copy, sizeof(copy), &copied) != UMI_STATUS_OK ||
        copied != 9U || strcmp(copy, "framework") != 0) return 7;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_text_buffer_mark_saved(buffer) != UMI_STATUS_OK ||
        umi_editor_text_buffer_is_dirty(buffer)) return 8;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_text_buffer_erase(buffer, 0U, 6U) != UMI_STATUS_OK ||
        !umi_editor_text_buffer_is_dirty(buffer)) return 9;
    umi_editor_text_buffer_destroy(buffer);
    return EXIT_SUCCESS;
}
