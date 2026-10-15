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
#include "../value_archive/construction_cases.h"
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

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiFabricConnectorBindingConditionalArchiveCases, UmiFabricConnectorBinding,
    umi_fabric_connector_binding_archive_encode, umi_fabric_connector_binding_archive_decode_if_unchanged,
    UmiFabricConnectorBindingTransferEqual, UmiFabricConnectorBindingTransferTails, UmiFabricConnectorBindingTransferMalformed)



/* A rejected replacement must preserve the previously accepted connector binding value. */
static int UmiFabricConnectorBindingConstructionCases(void)
{
    UmiFabricConnectorBinding *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_fabric_connector_binding_init(value, "b", "ibkr", "ep", "secure") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->binding_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_connector_binding_init(value, oversized, "ibkr", "ep", "secure") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_connector_binding_init(value, value->binding_id, "ibkr", "ep", "secure") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->binding_id, "b") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->connector_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_connector_binding_init(value, "b", oversized, "ep", "secure") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_connector_binding_init(value, "b", value->connector_id, "ep", "secure") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->connector_id, "ibkr") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->endpoint_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_connector_binding_init(value, "b", "ibkr", oversized, "secure") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_connector_binding_init(value, "b", "ibkr", value->endpoint_id, "secure") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->endpoint_id, "ep") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->transport_profile_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_connector_binding_init(value, "b", "ibkr", "ep", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_connector_binding_init(value, "b", "ibkr", "ep", value->transport_profile_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->transport_profile_id, "secure") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiFabricConnectorBindingConstructionCases() != 0) return 1;

    UmiFabricConnectorBinding item;
    CHECK(umi_fabric_connector_binding_init(&item,"b","ibkr","ep","secure")==UMI_STATUS_OK);
    if (UmiFabricConnectorBindingTransferCases(&item) != 0) return 1;
    if (UmiFabricConnectorBindingConditionalArchiveCases(&item) != 0) return 1;

    CHECK(item.enabled);
    return 0;
}
