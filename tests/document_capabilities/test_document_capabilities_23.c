/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_capabilities/test_document_capabilities_23.c
 *
 * PURPOSE:
 *   Verify document capability descriptor partition 23 and its public contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    const UmiDocumentCapabilityDescriptor *descriptor;
    char message[UMI_DOCUMENT_CAPABILITY_MESSAGE_CAPACITY];
    descriptor = umi_document_capability_ui_document_model();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_UI_DOCUMENT_MODEL) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_INTEGRATION);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_headless_document_host();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_HEADLESS_DOCUMENT_HOST) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_INTEGRATION);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_gtk4_document_host();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_GTK4_DOCUMENT_HOST) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_INTEGRATION);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_lsp_document_sync();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_LSP_DOCUMENT_SYNC) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_INTEGRATION);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    (void)printf("document capability partition 23 passed\n");
    return 0;
}
