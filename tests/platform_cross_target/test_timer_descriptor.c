/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_timer_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the timer descriptor cross-target capability.
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
#include "umicom/platform/cross_target/timer_descriptor.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/timer_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtTimerDescriptorTransferEqual(const UmiCtTimerDescriptor *a, const UmiCtTimerDescriptor *b)
{
    return strcmp(a->timer_id, b->timer_id) == 0 &&
        a->frequency_hz == b->frequency_hz &&
        a->counter_bits == b->counter_bits &&
        a->monotonic == b->monotonic &&
        a->oneshot == b->oneshot &&
        a->per_cpu == b->per_cpu;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtTimerDescriptorTransferTails(UmiCtTimerDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->timer_id) + 1U;
        memset(value->timer_id + used, 0xa5, sizeof(value->timer_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtTimerDescriptorTransferMalformed(const UmiCtTimerDescriptor *sample)
{
    (void)sample;
    {
        UmiCtTimerDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.timer_id, 'x', sizeof(invalid.timer_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_timer_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_timer_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated timer_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtTimerDescriptorTransferCases, UmiCtTimerDescriptor,
    umi_ct_timer_descriptor_archive_encode, umi_ct_timer_descriptor_archive_decode,
    UmiCtTimerDescriptorTransferEqual, UmiCtTimerDescriptorTransferTails, UmiCtTimerDescriptorTransferMalformed)

int main(void){UmiCtTimerDescriptor d={"mtime",UINT64_C(10000000),64U,true,true,false};CHECK(umi_ct_timer_descriptor_validate(&d)==UMI_STATUS_OK);
    if (UmiCtTimerDescriptorTransferCases(&d) != 0) return 1;
CHECK(umi_ct_timer_ns_to_ticks(&d,UINT64_C(1000000000))==UINT64_C(10000000));return 0;}
