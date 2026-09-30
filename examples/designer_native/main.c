/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/designer_native/main.c
 * PURPOSE:
 *   A source plan owns its text, not its original document.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A source plan owns its text, not its original document. */
#include "umicom/designer/native_project.h"
#include <stdio.h>
int main(void)
{
    UmiDeclDocument *document = NULL;
    UmiDesignerNativeProject *project = NULL;
    UmiDesignerNativeProjectSummary summary;
    char explanation[256] = "";
    UmiStatus status = UmiDesignerNativeNotesDocument(&document);
    if (status == UMI_STATUS_OK)
        status = UmiDesignerNativeProjectCreate(document, "umicom_notes", &project, explanation, sizeof explanation);
    /* Destroy the model before reading the generated plan to demonstrate its
     * independent lifetime. There is no hidden borrowed document pointer. */
    umi_decl_document_destroy(document);
    if (status == UMI_STATUS_OK) status = UmiDesignerNativeProjectGetSummary(project, &summary);
    if (status == UMI_STATUS_OK) {
        printf("Prepared %zu source files for %zu components.\n", summary.fileCount, summary.nodeCount);
        puts("The source plan remains readable after its original document is closed.\n"
             "Practice complete. Memory only; no files, compiler or window was opened.");
    } else fprintf(stderr, "%s: %s\n", umi_status_text(status), explanation);
    UmiDesignerNativeProjectDestroy(project);
    return status == UMI_STATUS_OK ? 0 : 1;
}
