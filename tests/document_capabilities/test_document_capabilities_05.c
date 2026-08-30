/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_capabilities/test_document_capabilities_05.c
 *
 * PURPOSE:
 *   Verify document capability descriptor partition 05 and its public contract.
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
    descriptor = umi_document_capability_local_file_provider();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_LOCAL_FILE_PROVIDER) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PROVIDER);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_memory_provider();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_MEMORY_PROVIDER) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PROVIDER);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_remote_provider();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_REMOTE_PROVIDER) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PROVIDER);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_archive_provider();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_ARCHIVE_PROVIDER) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PROVIDER);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    (void)printf("document capability partition 05 passed\n");
    return 0;
}
