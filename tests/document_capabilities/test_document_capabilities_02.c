/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_capabilities/test_document_capabilities_02.c
 *
 * PURPOSE:
 *   Verify document capability descriptor partition 02 and its public contract.
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
    descriptor = umi_document_capability_clone_document();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_CLONE_DOCUMENT) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_LIFECYCLE);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_read_only_document();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_READ_ONLY_DOCUMENT) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_LIFECYCLE);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_large_document();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_LARGE_DOCUMENT) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_LIFECYCLE);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_virtual_document();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_VIRTUAL_DOCUMENT) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_LIFECYCLE);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    (void)printf("document capability partition 02 passed\n");
    return 0;
}
