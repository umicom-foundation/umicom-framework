/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_capabilities/test_document_capabilities_06.c
 *
 * PURPOSE:
 *   Verify document capability descriptor partition 06 and its public contract.
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
    descriptor = umi_document_capability_ssh_provider();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_SSH_PROVIDER) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PROVIDER);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_webdav_provider();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_WEBDAV_PROVIDER) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PROVIDER);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_cloud_provider();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_CLOUD_PROVIDER) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PROVIDER);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_custom_provider();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_CUSTOM_PROVIDER) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PROVIDER);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    (void)printf("document capability partition 06 passed\n");
    return 0;
}
