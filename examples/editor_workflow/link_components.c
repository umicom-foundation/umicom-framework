/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/editor_workflow/link_components.c
 * PURPOSE:
 *   Check the public editor libraries without creating a window or touching
 *   source files. Demonstrate that an accepted step is not a completed close.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/close_session.h"
#include "umicom/document/save_session.h"
#include "umicom/ui/workbench.h"
#ifdef UMICOM_CHECK_NATIVE_EDITOR
#include "umicom/ui/gtk4/document_commands.h"
#endif
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    /* A real call creates a link dependency; checking a prototype alone does
     * not establish that the selected static library contains its definition. */
    UmiDocumentCloseSession *session = NULL;
    CHECK(UmiDocumentCloseSessionCreate(NULL, UMI_DOCUMENT_CLOSE_ALL, 0U,
        &session) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(session == NULL);
    UmiDocumentCloseSessionDestroy(session);
    CHECK(UmiUiWorkbenchClearClosedDocument(NULL, "notes.c") == UMI_STATUS_INVALID_ARGUMENT);

    /* These are sample observations, not a request to close a real document.
     * One Notes document is accounted for, and one still needs a decision. */
    UmiDocumentCloseProgress progress = {0};
    progress.phase = UMI_DOCUMENT_CLOSE_DECISION;
    progress.last_status = UMI_STATUS_OK;
    progress.total = 2U;
    progress.closed = 1U;
    progress.remaining = 1U;
    progress.current.document_id = 7U;
    progress.current.dirty = 1;
    memcpy(progress.current.display_name, "notes.h", sizeof("notes.h"));
    CHECK(UmiDocumentCloseProgressValidate(&progress) == UMI_STATUS_OK);
    CHECK(UmiDocumentCloseProgressRequireComplete(&progress) == UMI_STATUS_INVALID_STATE);
    char message[768];
    CHECK(UmiDocumentCloseProgressFormat(&progress, message, sizeof(message)) == UMI_STATUS_OK);
    puts(message);

    /* Cancellation remains cancellation; it is not converted to completion. */
    progress.phase = UMI_DOCUMENT_CLOSE_CANCELLED;
    progress.last_status = UMI_STATUS_CANCELLED;
    CHECK(UmiDocumentCloseProgressRequireComplete(&progress) == UMI_STATUS_CANCELLED);

    /* A fresh, fully completed observation can authorise a completion label. */
    progress.phase = UMI_DOCUMENT_CLOSE_COMPLETE;
    progress.last_status = UMI_STATUS_OK;
    progress.closed = 2U;
    progress.remaining = 0U;
    CHECK(UmiDocumentCloseProgressRequireComplete(&progress) == UMI_STATUS_OK);
    CHECK(UmiDocumentCloseProgressFormat(&progress, message, sizeof(message)) == UMI_STATUS_OK);
    puts(message);

    /* Retain real link dependencies on the earlier Save All corrections. */
    UmiDocumentSaveProgress save = {0};
    save.phase = UMI_DOCUMENT_SAVE_COMPLETE;
    save.last_status = UMI_STATUS_OK;
    CHECK(UmiDocumentSaveProgressValidate(&save) == UMI_STATUS_OK);
    CHECK(UmiDocumentSaveProgressFormat(&save, message, sizeof(message)) == UMI_STATUS_OK);
#ifdef UMICOM_CHECK_NATIVE_EDITOR
    /* This optional build links the actual GTK adapter library, but does not
     * initialise GTK or simulate a click. Argument checks cannot test a GUI. */
    CHECK(!UmiGtk4AdapterCloseDocumentsBusy(NULL));
    CHECK(!UmiGtk4AdapterDocumentCloseBusy(NULL));
    CHECK(UmiGtk4AdapterCloseDocuments(NULL, UMI_DOCUMENT_CLOSE_ALL, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4AdapterCancelCloseDocuments(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    UmiDocumentCloseProgress before;
    memcpy(&before, &progress, sizeof(before));
    CHECK(UmiGtk4AdapterCloseDocumentsProgress(NULL, &progress) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&before, &progress, sizeof(progress)) == 0);
    CHECK(UmiGtk4AdapterDocumentSaveAllProgress(NULL, &save) == UMI_STATUS_INVALID_ARGUMENT);
    puts("Native editor APIs linked; null-argument checks passed.");
#endif
    puts("Editor library calls and completion checks passed.");
    return 0;
}
