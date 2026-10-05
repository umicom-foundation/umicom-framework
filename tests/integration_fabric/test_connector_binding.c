/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_connector_binding.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the connector binding Integration Fabric capability.
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
#include "umicom/integration/fabric/connector_binding.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/connector_binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricConnectorBindingTransferEqual(const UmiFabricConnectorBinding *a, const UmiFabricConnectorBinding *b)
{
    return strcmp(a->binding_id, b->binding_id) == 0 &&
        strcmp(a->connector_id, b->connector_id) == 0 &&
        strcmp(a->endpoint_id, b->endpoint_id) == 0 &&
        strcmp(a->transport_profile_id, b->transport_profile_id) == 0 &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricConnectorBindingTransferTails(UmiFabricConnectorBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->binding_id) + 1U;
        memset(value->binding_id + used, 0xa5, sizeof(value->binding_id) - used);
    }
    {
        size_t used = strlen(value->connector_id) + 1U;
        memset(value->connector_id + used, 0xa5, sizeof(value->connector_id) - used);
    }
    {
        size_t used = strlen(value->endpoint_id) + 1U;
        memset(value->endpoint_id + used, 0xa5, sizeof(value->endpoint_id) - used);
    }
    {
        size_t used = strlen(value->transport_profile_id) + 1U;
        memset(value->transport_profile_id + used, 0xa5, sizeof(value->transport_profile_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricConnectorBindingTransferMalformed(const UmiFabricConnectorBinding *sample)
{
    (void)sample;
    {
        UmiFabricConnectorBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.binding_id, 'x', sizeof(invalid.binding_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_connector_binding_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_connector_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated binding_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricConnectorBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.connector_id, 'x', sizeof(invalid.connector_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_connector_binding_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_connector_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated connector_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricConnectorBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.endpoint_id, 'x', sizeof(invalid.endpoint_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_connector_binding_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_connector_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated endpoint_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricConnectorBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.transport_profile_id, 'x', sizeof(invalid.transport_profile_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_connector_binding_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_connector_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated transport_profile_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricConnectorBindingTransferCases, UmiFabricConnectorBinding,
    umi_fabric_connector_binding_archive_encode, umi_fabric_connector_binding_archive_decode,
    UmiFabricConnectorBindingTransferEqual, UmiFabricConnectorBindingTransferTails, UmiFabricConnectorBindingTransferMalformed)

int main(void) {
    UmiFabricConnectorBinding item;
    CHECK(umi_fabric_connector_binding_init(&item,"b","ibkr","ep","secure")==UMI_STATUS_OK);
    if (UmiFabricConnectorBindingTransferCases(&item) != 0) return 1;

    CHECK(item.enabled);
    return 0;
}
