/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_connector_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the connector descriptor Integration Fabric capability.
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
#include "umicom/integration/fabric/connector_descriptor.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/connector_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricConnectorDescriptorTransferEqual(const UmiFabricConnectorDescriptor *a, const UmiFabricConnectorDescriptor *b)
{
    return strcmp(a->connector_id, b->connector_id) == 0 &&
        strcmp(a->provider, b->provider) == 0 &&
        a->protocol == b->protocol &&
        a->capability_mask == b->capability_mask &&
        a->supports_transactions == b->supports_transactions;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricConnectorDescriptorTransferTails(UmiFabricConnectorDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->connector_id) + 1U;
        memset(value->connector_id + used, 0xa5, sizeof(value->connector_id) - used);
    }
    {
        size_t used = strlen(value->provider) + 1U;
        memset(value->provider + used, 0xa5, sizeof(value->provider) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricConnectorDescriptorTransferMalformed(const UmiFabricConnectorDescriptor *sample)
{
    (void)sample;
    {
        UmiFabricConnectorDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.connector_id, 'x', sizeof(invalid.connector_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_connector_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_connector_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated connector_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricConnectorDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.provider, 'x', sizeof(invalid.provider));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_connector_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_connector_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated provider was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricConnectorDescriptorTransferCases, UmiFabricConnectorDescriptor,
    umi_fabric_connector_descriptor_archive_encode, umi_fabric_connector_descriptor_archive_decode,
    UmiFabricConnectorDescriptorTransferEqual, UmiFabricConnectorDescriptorTransferTails, UmiFabricConnectorDescriptorTransferMalformed)

int main(void) {
    UmiFabricConnectorDescriptor item;
    CHECK(umi_fabric_connector_descriptor_init(&item,"ibkr","IBKR",UMI_FABRIC_PROTOCOL_TCP,7U,false)==UMI_STATUS_OK);
    if (UmiFabricConnectorDescriptorTransferCases(&item) != 0) return 1;

    CHECK(strcmp(item.provider,"IBKR")==0);
    return 0;
}
