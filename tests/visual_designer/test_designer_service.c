/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_designer_service.c
 *
 * PURPOSE:
 *   Validate aggregate visual designer readiness and active-session state for thin frontends.
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
#include "umicom/designer/visual_designer/designer_service.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/designer_service.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadDesignerServiceTransferEqual(const UmiRadDesignerService *a, const UmiRadDesignerService *b)
{
    return a->active_sessions == b->active_sessions &&
        a->open_documents == b->open_documents &&
        a->conformance_score == b->conformance_score &&
        a->initialized == b->initialized;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadDesignerServiceTransferTails(UmiRadDesignerService *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadDesignerServiceTransferMalformed(const UmiRadDesignerService *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadDesignerServiceTransferCases, UmiRadDesignerService,
    umi_rad_designer_service_archive_encode, umi_rad_designer_service_archive_decode,
    UmiRadDesignerServiceTransferEqual, UmiRadDesignerServiceTransferTails, UmiRadDesignerServiceTransferMalformed)

int main(void){UmiRadDesignerService item;CHECK(umi_rad_designer_service_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_designer_service_is_valid(&item));
    if (UmiRadDesignerServiceTransferCases(&item) != 0) return 1;
return 0;}
