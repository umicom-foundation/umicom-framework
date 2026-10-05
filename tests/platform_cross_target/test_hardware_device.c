/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_hardware_device.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the hardware device cross-target capability.
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
#include "umicom/platform/cross_target/hardware_device.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/hardware_device.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtHardwareDeviceTransferEqual(const UmiCtHardwareDevice *a, const UmiCtHardwareDevice *b)
{
    return strcmp(a->device_id, b->device_id) == 0 &&
        strcmp(a->compatible, b->compatible) == 0 &&
        a->device_class == b->device_class &&
        a->mmio_base == b->mmio_base &&
        a->mmio_size == b->mmio_size &&
        a->irq == b->irq &&
        a->present == b->present;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtHardwareDeviceTransferTails(UmiCtHardwareDevice *value)
{
    (void)value;
    {
        size_t used = strlen(value->device_id) + 1U;
        memset(value->device_id + used, 0xa5, sizeof(value->device_id) - used);
    }
    {
        size_t used = strlen(value->compatible) + 1U;
        memset(value->compatible + used, 0xa5, sizeof(value->compatible) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtHardwareDeviceTransferMalformed(const UmiCtHardwareDevice *sample)
{
    (void)sample;
    {
        UmiCtHardwareDevice invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.device_id, 'x', sizeof(invalid.device_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_hardware_device_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_hardware_device_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated device_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCtHardwareDevice invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.compatible, 'x', sizeof(invalid.compatible));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_hardware_device_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_hardware_device_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated compatible was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtHardwareDeviceTransferCases, UmiCtHardwareDevice,
    umi_ct_hardware_device_archive_encode, umi_ct_hardware_device_archive_decode,
    UmiCtHardwareDeviceTransferEqual, UmiCtHardwareDeviceTransferTails, UmiCtHardwareDeviceTransferMalformed)

int main(void){UmiCtHardwareDevice d={"uart0","ns16550a",UMI_CT_DEVICE_OTHER,UINT64_C(0x10000000),0x100U,10U,true};CHECK(umi_ct_hardware_device_validate(&d)==UMI_STATUS_OK);
    if (UmiCtHardwareDeviceTransferCases(&d) != 0) return 1;
return 0;}
