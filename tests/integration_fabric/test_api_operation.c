/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_api_operation.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the api operation Integration Fabric capability.
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
#include "umicom/integration/fabric/api_operation.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/api_operation.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricApiOperationTransferEqual(const UmiFabricApiOperation *a, const UmiFabricApiOperation *b)
{
    return strcmp(a->operation_id, b->operation_id) == 0 &&
        strcmp(a->method, b->method) == 0 &&
        strcmp(a->path, b->path) == 0 &&
        strcmp(a->request_schema, b->request_schema) == 0 &&
        strcmp(a->response_schema, b->response_schema) == 0 &&
        a->idempotent == b->idempotent;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricApiOperationTransferTails(UmiFabricApiOperation *value)
{
    (void)value;
    {
        size_t used = strlen(value->operation_id) + 1U;
        memset(value->operation_id + used, 0xa5, sizeof(value->operation_id) - used);
    }
    {
        size_t used = strlen(value->method) + 1U;
        memset(value->method + used, 0xa5, sizeof(value->method) - used);
    }
    {
        size_t used = strlen(value->path) + 1U;
        memset(value->path + used, 0xa5, sizeof(value->path) - used);
    }
    {
        size_t used = strlen(value->request_schema) + 1U;
        memset(value->request_schema + used, 0xa5, sizeof(value->request_schema) - used);
    }
    {
        size_t used = strlen(value->response_schema) + 1U;
        memset(value->response_schema + used, 0xa5, sizeof(value->response_schema) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricApiOperationTransferMalformed(const UmiFabricApiOperation *sample)
{
    (void)sample;
    {
        UmiFabricApiOperation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.operation_id, 'x', sizeof(invalid.operation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_api_operation_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_api_operation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated operation_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricApiOperation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.method, 'x', sizeof(invalid.method));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_api_operation_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_api_operation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated method was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricApiOperation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.path, 'x', sizeof(invalid.path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_api_operation_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_api_operation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricApiOperation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.request_schema, 'x', sizeof(invalid.request_schema));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_api_operation_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_api_operation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated request_schema was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricApiOperation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.response_schema, 'x', sizeof(invalid.response_schema));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_api_operation_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_api_operation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated response_schema was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricApiOperationTransferCases, UmiFabricApiOperation,
    umi_fabric_api_operation_archive_encode, umi_fabric_api_operation_archive_decode,
    UmiFabricApiOperationTransferEqual, UmiFabricApiOperationTransferTails, UmiFabricApiOperationTransferMalformed)

int main(void) {
    UmiFabricApiOperation item;
    CHECK(umi_fabric_api_operation_init(&item,"get.order","GET","/orders/{id}","none","order",true)==UMI_STATUS_OK);
    if (UmiFabricApiOperationTransferCases(&item) != 0) return 1;

    CHECK(item.idempotent);
    return 0;
}
