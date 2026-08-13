/*-----------------------------------------------------------------------------
 * Umicom Framework text buffer tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

#include "umicom/editor/text_buffer.h"

int main(void)
{
    UmiEditorTextBuffer *buffer = NULL;
    UmiEditorTextBufferView view;
    char copy[64];
    size_t copied = 0U;

    if (umi_editor_text_buffer_create(4U, &buffer) != UMI_STATUS_OK) return 1;
    if (umi_editor_text_buffer_set(buffer, "alpha\nbeta", 10U) != UMI_STATUS_OK) return 2;
    if (umi_editor_text_buffer_insert(buffer, 5U, " framework", 10U) != UMI_STATUS_OK) return 3;
    if (umi_editor_text_buffer_replace(buffer, 16U, 4U, "editor", 6U) != UMI_STATUS_OK) return 4;
    if (umi_editor_text_buffer_view(buffer, &view) != UMI_STATUS_OK) return 5;
    if (view.byte_count != 22U || !view.dirty ||
        memcmp(view.bytes, "alpha framework\neditor", 22U) != 0) return 6;
    if (umi_editor_text_buffer_copy(buffer, 6U, 9U, copy, sizeof(copy), &copied) != UMI_STATUS_OK ||
        copied != 9U || strcmp(copy, "framework") != 0) return 7;
    if (umi_editor_text_buffer_mark_saved(buffer) != UMI_STATUS_OK ||
        umi_editor_text_buffer_is_dirty(buffer)) return 8;
    if (umi_editor_text_buffer_erase(buffer, 0U, 6U) != UMI_STATUS_OK ||
        !umi_editor_text_buffer_is_dirty(buffer)) return 9;
    umi_editor_text_buffer_destroy(buffer);
    return EXIT_SUCCESS;
}
