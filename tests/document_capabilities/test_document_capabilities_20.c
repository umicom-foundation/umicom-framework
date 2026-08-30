/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_capabilities/test_document_capabilities_20.c
 *
 * PURPOSE:
 *   Verify document capability descriptor partition 20 and its public contract.
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
    descriptor = umi_document_capability_binary_file_policy();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_BINARY_FILE_POLICY) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_SECURITY);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_write_permission_policy();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_WRITE_PERMISSION_POLICY) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_SECURITY);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_document_audit();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_DOCUMENT_AUDIT) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_SECURITY);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_sensitive_content_redaction();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_SENSITIVE_CONTENT_REDACTION) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_SECURITY);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    (void)printf("document capability partition 20 passed\n");
    return 0;
}
