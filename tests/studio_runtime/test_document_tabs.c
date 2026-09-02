/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_document_tabs.c
 *
 * PURPOSE:
 *   Verify document-tab initialization and coordinator validation contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio_runtime/document_tabs.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiStudioRuntimeDocumentTabs tabs;

    umi_studio_document_tabs_init(&tabs);
    assert(tabs.count == 0U);
    assert(!tabs.has_active);
    assert(umi_studio_document_tabs_refresh(
        &tabs, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
