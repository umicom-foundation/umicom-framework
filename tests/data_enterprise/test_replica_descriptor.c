/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_replica_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the replica descriptor enterprise data capability.
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
#include "umicom/data/enterprise/replica_descriptor.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/replica_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataReplicaDescriptorTransferEqual(const UmiDataReplicaDescriptor *a, const UmiDataReplicaDescriptor *b)
{
    return strcmp(a->replica_id, b->replica_id) == 0 &&
        strcmp(a->endpoint, b->endpoint) == 0 &&
        a->priority == b->priority &&
        a->primary == b->primary &&
        a->healthy == b->healthy &&
        a->writable == b->writable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataReplicaDescriptorTransferTails(UmiDataReplicaDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->replica_id) + 1U;
        memset(value->replica_id + used, 0xa5, sizeof(value->replica_id) - used);
    }
    {
        size_t used = strlen(value->endpoint) + 1U;
        memset(value->endpoint + used, 0xa5, sizeof(value->endpoint) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataReplicaDescriptorTransferMalformed(const UmiDataReplicaDescriptor *sample)
{
    (void)sample;
    {
        UmiDataReplicaDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.replica_id, 'x', sizeof(invalid.replica_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_replica_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_replica_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated replica_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataReplicaDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.endpoint, 'x', sizeof(invalid.endpoint));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_replica_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_replica_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated endpoint was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataReplicaDescriptorTransferCases, UmiDataReplicaDescriptor,
    umi_data_replica_descriptor_archive_encode, umi_data_replica_descriptor_archive_decode,
    UmiDataReplicaDescriptorTransferEqual, UmiDataReplicaDescriptorTransferTails, UmiDataReplicaDescriptorTransferMalformed)

int main(void) {
    UmiDataReplicaDescriptor item;
    CHECK(umi_data_replica_descriptor_init(&item,"r1","db://primary",1U,true) == UMI_STATUS_OK);
    if (UmiDataReplicaDescriptorTransferCases(&item) != 0) return 1;

    CHECK(item.primary && item.writable);
    return 0;
}
