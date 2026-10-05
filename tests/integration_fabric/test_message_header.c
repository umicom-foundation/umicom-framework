/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_message_header.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the message header Integration Fabric capability.
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
#include "umicom/integration/fabric/message_header.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/message_header.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricMessageHeaderTransferEqual(const UmiFabricMessageHeader *a, const UmiFabricMessageHeader *b)
{
    return strcmp(a->message_id, b->message_id) == 0 &&
        strcmp(a->correlation_id, b->correlation_id) == 0 &&
        strcmp(a->causation_id, b->causation_id) == 0 &&
        strcmp(a->tenant_id, b->tenant_id) == 0 &&
        strcmp(a->content_type, b->content_type) == 0 &&
        a->created_ms == b->created_ms;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricMessageHeaderTransferTails(UmiFabricMessageHeader *value)
{
    (void)value;
    {
        size_t used = strlen(value->message_id) + 1U;
        memset(value->message_id + used, 0xa5, sizeof(value->message_id) - used);
    }
    {
        size_t used = strlen(value->correlation_id) + 1U;
        memset(value->correlation_id + used, 0xa5, sizeof(value->correlation_id) - used);
    }
    {
        size_t used = strlen(value->causation_id) + 1U;
        memset(value->causation_id + used, 0xa5, sizeof(value->causation_id) - used);
    }
    {
        size_t used = strlen(value->tenant_id) + 1U;
        memset(value->tenant_id + used, 0xa5, sizeof(value->tenant_id) - used);
    }
    {
        size_t used = strlen(value->content_type) + 1U;
        memset(value->content_type + used, 0xa5, sizeof(value->content_type) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricMessageHeaderTransferMalformed(const UmiFabricMessageHeader *sample)
{
    (void)sample;
    {
        UmiFabricMessageHeader invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.message_id, 'x', sizeof(invalid.message_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_message_header_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_message_header_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated message_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricMessageHeader invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.correlation_id, 'x', sizeof(invalid.correlation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_message_header_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_message_header_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated correlation_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricMessageHeader invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.causation_id, 'x', sizeof(invalid.causation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_message_header_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_message_header_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated causation_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricMessageHeader invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.tenant_id, 'x', sizeof(invalid.tenant_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_message_header_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_message_header_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated tenant_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricMessageHeader invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.content_type, 'x', sizeof(invalid.content_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_message_header_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_message_header_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated content_type was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricMessageHeaderTransferCases, UmiFabricMessageHeader,
    umi_fabric_message_header_archive_encode, umi_fabric_message_header_archive_decode,
    UmiFabricMessageHeaderTransferEqual, UmiFabricMessageHeaderTransferTails, UmiFabricMessageHeaderTransferMalformed)

int main(void) {
    UmiFabricMessageHeader item;
    CHECK(umi_fabric_message_header_init(&item,"m1","c1","tenant","application/json",100U)==UMI_STATUS_OK);
    if (UmiFabricMessageHeaderTransferCases(&item) != 0) return 1;

    CHECK(strcmp(item.correlation_id,"c1")==0);
    return 0;
}
