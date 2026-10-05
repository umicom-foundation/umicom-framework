/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_render_context.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web render context.
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
#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/render_context.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/frontend/native_web/render_context.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiNativeWebRenderContextTransferEqual(const UmiNativeWebRenderContext *a, const UmiNativeWebRenderContext *b)
{
    return strcmp(a->session_id, b->session_id) == 0 &&
        strcmp(a->route, b->route) == 0 &&
        strcmp(a->theme, b->theme) == 0 &&
        strcmp(a->density, b->density) == 0 &&
        strcmp(a->locale, b->locale) == 0 &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiNativeWebRenderContextTransferTails(UmiNativeWebRenderContext *value)
{
    (void)value;
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
    {
        size_t used = strlen(value->route) + 1U;
        memset(value->route + used, 0xa5, sizeof(value->route) - used);
    }
    {
        size_t used = strlen(value->theme) + 1U;
        memset(value->theme + used, 0xa5, sizeof(value->theme) - used);
    }
    {
        size_t used = strlen(value->density) + 1U;
        memset(value->density + used, 0xa5, sizeof(value->density) - used);
    }
    {
        size_t used = strlen(value->locale) + 1U;
        memset(value->locale + used, 0xa5, sizeof(value->locale) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiNativeWebRenderContextTransferMalformed(const UmiNativeWebRenderContext *sample)
{
    (void)sample;
    {
        UmiNativeWebRenderContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.session_id, 'x', sizeof(invalid.session_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_native_web_render_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_native_web_render_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated session_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiNativeWebRenderContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.route, 'x', sizeof(invalid.route));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_native_web_render_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_native_web_render_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated route was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiNativeWebRenderContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.theme, 'x', sizeof(invalid.theme));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_native_web_render_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_native_web_render_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated theme was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiNativeWebRenderContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.density, 'x', sizeof(invalid.density));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_native_web_render_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_native_web_render_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated density was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiNativeWebRenderContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.locale, 'x', sizeof(invalid.locale));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_native_web_render_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_native_web_render_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated locale was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiNativeWebRenderContextTransferCases, UmiNativeWebRenderContext,
    umi_native_web_render_context_archive_encode, umi_native_web_render_context_archive_decode,
    UmiNativeWebRenderContextTransferEqual, UmiNativeWebRenderContextTransferTails, UmiNativeWebRenderContextTransferMalformed)

int main(void)
{
    UmiNativeWebRenderContext c; CHECK(umi_native_web_render_context_init(&c,"s1","/")==UMI_STATUS_OK); CHECK(umi_native_web_render_context_validate(&c)==UMI_STATUS_OK);
    if (UmiNativeWebRenderContextTransferCases(&c) != 0) return 1;
 CHECK(strcmp(c.locale,"en-GB")==0);
    return 0;
}
