/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_boot_service.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the boot service cross-target capability.
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
#include "umicom/platform/cross_target/boot_service.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/boot_service.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtBootServiceTransferEqual(const UmiCtBootService *a, const UmiCtBootService *b)
{
    return strcmp(a->service_id, b->service_id) == 0 &&
        a->phase == b->phase &&
        a->essential == b->essential &&
        a->timeout_ms == b->timeout_ms;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtBootServiceTransferTails(UmiCtBootService *value)
{
    (void)value;
    {
        size_t used = strlen(value->service_id) + 1U;
        memset(value->service_id + used, 0xa5, sizeof(value->service_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtBootServiceTransferMalformed(const UmiCtBootService *sample)
{
    (void)sample;
    {
        UmiCtBootService invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.service_id, 'x', sizeof(invalid.service_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_boot_service_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_boot_service_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated service_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtBootServiceTransferCases, UmiCtBootService,
    umi_ct_boot_service_archive_encode, umi_ct_boot_service_archive_decode,
    UmiCtBootServiceTransferEqual, UmiCtBootServiceTransferTails, UmiCtBootServiceTransferMalformed)

int main(void){UmiCtBootService k={"kernel",UMI_CT_BOOT_EARLY,true,1000U},n={"net",UMI_CT_BOOT_SERVICES,true,2000U};CHECK(umi_ct_boot_service_validate(&k)==UMI_STATUS_OK);
    if (UmiCtBootServiceTransferCases(&k) != 0) return 1;
CHECK(umi_ct_boot_dependency_phase_valid(&n,&k));CHECK(!umi_ct_boot_dependency_phase_valid(&k,&n));return 0;}
