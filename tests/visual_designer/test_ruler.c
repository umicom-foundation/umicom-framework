/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_ruler.c
 *
 * PURPOSE:
 *   Validate describe design-time rulers and origin offsets.
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
#include "umicom/designer/visual_designer/ruler.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/ruler.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadRulerTransferEqual(const UmiRadRuler *a, const UmiRadRuler *b)
{
    return a->orientation == b->orientation &&
        a->origin == b->origin &&
        a->major_step == b->major_step &&
        a->visible == b->visible;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadRulerTransferTails(UmiRadRuler *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadRulerTransferMalformed(const UmiRadRuler *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadRulerTransferCases, UmiRadRuler,
    umi_rad_ruler_archive_encode, umi_rad_ruler_archive_decode,
    UmiRadRulerTransferEqual, UmiRadRulerTransferTails, UmiRadRulerTransferMalformed)

int main(void){UmiRadRuler item;CHECK(umi_rad_ruler_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_ruler_is_valid(&item));
    if (UmiRadRulerTransferCases(&item) != 0) return 1;
return 0;}
