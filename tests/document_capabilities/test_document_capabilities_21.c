/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_capabilities/test_document_capabilities_21.c
 *
 * PURPOSE:
 *   Verify document capability descriptor partition 21 and its public contract.
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
    descriptor = umi_document_capability_memory_mapping();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_MEMORY_MAPPING) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PERFORMANCE);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_streaming_loader();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_STREAMING_LOADER) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PERFORMANCE);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_incremental_fingerprint();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_INCREMENTAL_FINGERPRINT) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PERFORMANCE);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_asynchronous_load();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_ASYNCHRONOUS_LOAD) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PERFORMANCE);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    (void)printf("document capability partition 21 passed\n");
    return 0;
}
