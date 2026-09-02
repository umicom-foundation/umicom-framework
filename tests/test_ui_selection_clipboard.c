/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_selection_clipboard.c
 *
 * PURPOSE:
 *   Verify multi-selection and in-memory clipboard state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiSelectionModel *selection = NULL;
    UmiUiClipboard *clipboard = NULL;
    char text[128];
    assert(umi_ui_selection_create(&selection) == UMI_STATUS_OK);
    assert(umi_ui_selection_add(selection, "document.one") == UMI_STATUS_OK);
    assert(umi_ui_selection_add(selection, "document.two") == UMI_STATUS_OK);
    assert(umi_ui_selection_contains(selection, "document.two"));
    assert(umi_ui_selection_remove(selection, "document.one") == UMI_STATUS_OK);
    assert(umi_ui_clipboard_create(&clipboard) == UMI_STATUS_OK);
    assert(umi_ui_clipboard_set_text(clipboard, "Umicom clipboard") == UMI_STATUS_OK);
    assert(umi_ui_clipboard_get_text(clipboard, text, sizeof(text)) == UMI_STATUS_OK);
    assert(strcmp(text, "Umicom clipboard") == 0);
    umi_ui_clipboard_destroy(clipboard);
    umi_ui_selection_destroy(selection);
    return EXIT_SUCCESS;
}
