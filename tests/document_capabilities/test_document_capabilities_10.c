/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_capabilities/test_document_capabilities_10.c
 *
 * PURPOSE:
 *   Verify document capability descriptor partition 10 and its public contract.
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
    descriptor = umi_document_capability_permission_preservation();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_PERMISSION_PRESERVATION) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PERSISTENCE);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_line_ending_policy();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_LINE_ENDING_POLICY) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PERSISTENCE);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_final_newline_policy();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_FINAL_NEWLINE_POLICY) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PERSISTENCE);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_document_capability_trailing_whitespace_policy();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DOCUMENT_CAPABILITY_TRAILING_WHITESPACE_POLICY) == 0);
    assert(descriptor->category == UMI_DOCUMENT_CAPABILITY_CATEGORY_PERSISTENCE);
    assert(umi_document_capability_descriptor_validate(descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    (void)printf("document capability partition 10 passed\n");
    return 0;
}
