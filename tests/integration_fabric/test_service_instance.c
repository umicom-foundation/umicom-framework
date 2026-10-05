/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_service_instance.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the service instance Integration Fabric capability.
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
#include "umicom/integration/fabric/service_instance.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/service_instance.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricServiceInstanceTransferEqual(const UmiFabricServiceInstance *a, const UmiFabricServiceInstance *b)
{
    return strcmp(a->instance_id, b->instance_id) == 0 &&
        strcmp(a->service_id, b->service_id) == 0 &&
        strcmp(a->endpoint_id, b->endpoint_id) == 0 &&
        a->priority == b->priority &&
        a->weight == b->weight &&
        a->healthy == b->healthy &&
        a->last_seen_ms == b->last_seen_ms;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricServiceInstanceTransferTails(UmiFabricServiceInstance *value)
{
    (void)value;
    {
        size_t used = strlen(value->instance_id) + 1U;
        memset(value->instance_id + used, 0xa5, sizeof(value->instance_id) - used);
    }
    {
        size_t used = strlen(value->service_id) + 1U;
        memset(value->service_id + used, 0xa5, sizeof(value->service_id) - used);
    }
    {
        size_t used = strlen(value->endpoint_id) + 1U;
        memset(value->endpoint_id + used, 0xa5, sizeof(value->endpoint_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricServiceInstanceTransferMalformed(const UmiFabricServiceInstance *sample)
{
    (void)sample;
    {
        UmiFabricServiceInstance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instance_id, 'x', sizeof(invalid.instance_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_service_instance_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_service_instance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instance_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricServiceInstance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.service_id, 'x', sizeof(invalid.service_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_service_instance_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_service_instance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated service_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricServiceInstance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.endpoint_id, 'x', sizeof(invalid.endpoint_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_service_instance_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_service_instance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated endpoint_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricServiceInstanceTransferCases, UmiFabricServiceInstance,
    umi_fabric_service_instance_archive_encode, umi_fabric_service_instance_archive_decode,
    UmiFabricServiceInstanceTransferEqual, UmiFabricServiceInstanceTransferTails, UmiFabricServiceInstanceTransferMalformed)

int main(void) {
    UmiFabricServiceInstance item;
    CHECK(umi_fabric_service_instance_init(&item,"risk.1","risk","ep.1",1U,10U)==UMI_STATUS_OK);
    if (UmiFabricServiceInstanceTransferCases(&item) != 0) return 1;

    CHECK(item.healthy);
    return 0;
}
