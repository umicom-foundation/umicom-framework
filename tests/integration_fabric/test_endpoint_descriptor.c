/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_endpoint_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the endpoint descriptor Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/integration/fabric/endpoint_descriptor.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/endpoint_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricEndpointDescriptorTransferEqual(const UmiFabricEndpointDescriptor *a, const UmiFabricEndpointDescriptor *b)
{
    return strcmp(a->endpoint_id, b->endpoint_id) == 0 &&
        strcmp(a->uri, b->uri) == 0 &&
        a->protocol == b->protocol &&
        a->secure == b->secure &&
        a->weight == b->weight;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricEndpointDescriptorTransferTails(UmiFabricEndpointDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->endpoint_id) + 1U;
        memset(value->endpoint_id + used, 0xa5, sizeof(value->endpoint_id) - used);
    }
    {
        size_t used = strlen(value->uri) + 1U;
        memset(value->uri + used, 0xa5, sizeof(value->uri) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricEndpointDescriptorTransferMalformed(const UmiFabricEndpointDescriptor *sample)
{
    (void)sample;
    {
        UmiFabricEndpointDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.endpoint_id, 'x', sizeof(invalid.endpoint_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_endpoint_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_endpoint_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated endpoint_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricEndpointDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.uri, 'x', sizeof(invalid.uri));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_endpoint_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_endpoint_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated uri was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricEndpointDescriptorTransferCases, UmiFabricEndpointDescriptor,
    umi_fabric_endpoint_descriptor_archive_encode, umi_fabric_endpoint_descriptor_archive_decode,
    UmiFabricEndpointDescriptorTransferEqual, UmiFabricEndpointDescriptorTransferTails, UmiFabricEndpointDescriptorTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiFabricEndpointDescriptorTransferCasesWithWorkspace, UmiFabricEndpointDescriptor,
    umi_fabric_endpoint_descriptor_archive_encode, umi_fabric_endpoint_descriptor_archive_decode_with_workspace, UmiFabricEndpointDescriptorTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiFabricEndpointDescriptorConditionalArchiveCases, UmiFabricEndpointDescriptor,
    umi_fabric_endpoint_descriptor_archive_encode, umi_fabric_endpoint_descriptor_archive_decode_if_unchanged,
    UmiFabricEndpointDescriptorTransferEqual, UmiFabricEndpointDescriptorTransferTails, UmiFabricEndpointDescriptorTransferMalformed)



/* A rejected replacement must preserve the previously accepted endpoint descriptor value. */
static int UmiFabricEndpointDescriptorConstructionCases(void)
{
    UmiFabricEndpointDescriptor *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_fabric_endpoint_descriptor_init(value, "e1", "https://risk", UMI_FABRIC_PROTOCOL_HTTP, true, 10U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->endpoint_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_endpoint_descriptor_init(value, oversized, "https://risk", UMI_FABRIC_PROTOCOL_HTTP, true, 10U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_endpoint_descriptor_init(value, value->endpoint_id, "https://risk", UMI_FABRIC_PROTOCOL_HTTP, true, 10U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->endpoint_id, "e1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->uri) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_endpoint_descriptor_init(value, "e1", oversized, UMI_FABRIC_PROTOCOL_HTTP, true, 10U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_endpoint_descriptor_init(value, "e1", value->uri, UMI_FABRIC_PROTOCOL_HTTP, true, 10U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->uri, "https://risk") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiFabricEndpointDescriptorConstructionCases() != 0) return 1;

    UmiFabricEndpointDescriptor item;
    CHECK(umi_fabric_endpoint_descriptor_init(&item,"e1","https://risk",UMI_FABRIC_PROTOCOL_HTTP,true,10U)==UMI_STATUS_OK);
    if (UmiFabricEndpointDescriptorTransferCases(&item) != 0) return 1;
    if (UmiFabricEndpointDescriptorTransferCasesWithWorkspace(&item) != 0) return 1;
    if (UmiFabricEndpointDescriptorConditionalArchiveCases(&item) != 0) return 1;

    CHECK(item.secure);
    return 0;
}
