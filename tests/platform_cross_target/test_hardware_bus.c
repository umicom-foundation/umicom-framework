/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_hardware_bus.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the hardware bus cross-target capability.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
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
#include "umicom/platform/cross_target/hardware_bus.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/hardware_bus.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtHardwareBusTransferEqual(const UmiCtHardwareBus *a, const UmiCtHardwareBus *b)
{
    return strcmp(a->bus_id, b->bus_id) == 0 &&
        a->type == b->type &&
        a->enumerable == b->enumerable &&
        a->hotplug == b->hotplug &&
        a->dma == b->dma &&
        a->address_bits == b->address_bits;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtHardwareBusTransferTails(UmiCtHardwareBus *value)
{
    (void)value;
    {
        size_t used = strlen(value->bus_id) + 1U;
        memset(value->bus_id + used, 0xa5, sizeof(value->bus_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtHardwareBusTransferMalformed(const UmiCtHardwareBus *sample)
{
    (void)sample;
    {
        UmiCtHardwareBus invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.bus_id, 'x', sizeof(invalid.bus_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_hardware_bus_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_hardware_bus_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated bus_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtHardwareBusTransferCases, UmiCtHardwareBus,
    umi_ct_hardware_bus_archive_encode, umi_ct_hardware_bus_archive_decode,
    UmiCtHardwareBusTransferEqual, UmiCtHardwareBusTransferTails, UmiCtHardwareBusTransferMalformed)

int main(void){UmiCtHardwareBus b={"virtio",UMI_CT_BUS_VIRTIO,true,true,true,64U};CHECK(umi_ct_hardware_bus_validate(&b)==UMI_STATUS_OK);
    if (UmiCtHardwareBusTransferCases(&b) != 0) return 1;
b.address_bits=24U;CHECK(umi_ct_hardware_bus_validate(&b)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
