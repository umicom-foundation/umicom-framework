/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_design_token.c
 *
 * PURPOSE:
 *   Verify typed design-token construction and validation.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
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
#include "umicom/ui/design/design_token.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/design_token.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignTokenTransferEqual(const UmiDesignToken *a, const UmiDesignToken *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->kind == b->kind &&
        a->color.red == b->color.red &&
        a->color.green == b->color.green &&
        a->color.blue == b->color.blue &&
        a->color.alpha == b->color.alpha &&
        a->number == b->number &&
        a->integer == b->integer &&
        a->length.value == b->length.value &&
        a->length.unit == b->length.unit &&
        strcmp(a->text, b->text) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignTokenTransferTails(UmiDesignToken *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->text) + 1U;
        memset(value->text + used, 0xa5, sizeof(value->text) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignTokenTransferMalformed(const UmiDesignToken *sample)
{
    (void)sample;
    {
        UmiDesignToken invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_design_token_valid(&invalid)) ||
            umi_design_token_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDesignToken invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.text, 'x', sizeof(invalid.text));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_design_token_valid(&invalid)) ||
            umi_design_token_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated text was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignTokenTransferCases, UmiDesignToken,
    umi_design_token_archive_encode, umi_design_token_archive_decode,
    UmiDesignTokenTransferEqual, UmiDesignTokenTransferTails, UmiDesignTokenTransferMalformed)

int main(void){UmiDesignToken t;UmiDesignRgba c;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_color_make(0.2,0.6,0.9,1.0,&c)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_token_color(&t,"accent",c)!=UMI_STATUS_OK)return 2;
    if (UmiDesignTokenTransferCases(&t) != 0) return 1;
return umi_design_token_valid(&t)?0:3;}
