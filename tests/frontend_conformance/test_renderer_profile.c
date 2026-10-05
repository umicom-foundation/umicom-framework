/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_renderer_profile.c
 *
 * PURPOSE:
 *   Focused regression coverage for renderer identity, capability and policy metadata used by conformance evaluation.
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
#include "umicom/frontend/conformance/renderer_profile.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/frontend/conformance/renderer_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFcRendererProfileTransferEqual(const UmiFcRendererProfile *a, const UmiFcRendererProfile *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->kind == b->kind &&
        a->capabilities == b->capabilities &&
        a->api_version == b->api_version &&
        a->production_ready == b->production_ready &&
        a->remote_session == b->remote_session;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFcRendererProfileTransferTails(UmiFcRendererProfile *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFcRendererProfileTransferMalformed(const UmiFcRendererProfile *sample)
{
    (void)sample;
    {
        UmiFcRendererProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fc_renderer_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fc_renderer_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFcRendererProfileTransferCases, UmiFcRendererProfile,
    umi_fc_renderer_profile_archive_encode, umi_fc_renderer_profile_archive_decode,
    UmiFcRendererProfileTransferEqual, UmiFcRendererProfileTransferTails, UmiFcRendererProfileTransferMalformed)

int main(void) {
    UmiFcRendererProfile p; CHECK(umi_fc_renderer_profile_make("gtk4",UMI_FC_FRONTEND_GTK4,UINT64_C(7),1U,&p)==UMI_STATUS_OK); CHECK(p.kind==UMI_FC_FRONTEND_GTK4); CHECK(umi_fc_renderer_profile_validate(&p)==UMI_STATUS_OK);
    if (UmiFcRendererProfileTransferCases(&p) != 0) return 1;

    return 0;
}
