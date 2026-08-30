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
int main(void)
{
    UmiStudioRuntimeDocumentState documents;
    UmiStudioRuntimeWindowTitle title;
    umi_studio_document_state_init(&documents);
    documents.has_active=1;
    documents.active.dirty=1;
    (void)strcpy(documents.active.display_name,"main.c");
    assert(umi_studio_window_title_build(
        "Umicom Studio","workspace",&documents,&title)==UMI_STATUS_OK);
    assert(strstr(title.title,"main.c *")!=NULL);
    assert(strstr(title.title,"workspace")!=NULL);
    return 0;
}

