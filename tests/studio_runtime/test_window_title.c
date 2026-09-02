/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_window_title.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime window title.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/window_title.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiStudioRuntimeDocumentState documents;
    UmiStudioRuntimeWindowTitle title;
    umi_studio_document_state_init(&documents);
    documents.has_active=1;
    documents.active.dirty=1;
    (void)strcpy(documents.active.display_name,"main.c");
    assert(umi_studio_window_title_build(
        "Umicom Studio IDE","workspace",&documents,&title)==UMI_STATUS_OK);
    /* The product identity leads the native title, followed by document and
     * workspace context. This keeps the correct icon/name pair recognisable. */
    assert(strstr(title.title, "Umicom Studio IDE — ") == title.title);
    assert(strstr(title.title,"main.c *")!=NULL);
    assert(strstr(title.title,"workspace")!=NULL);
    return 0;
}
