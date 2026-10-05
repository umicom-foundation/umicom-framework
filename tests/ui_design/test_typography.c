/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_typography.c
 *
 * PURPOSE:
 *   Verify typography family, size, weight and line-height validation.
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
#include "umicom/ui/design/typography.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/typography.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignTypographyTransferEqual(const UmiDesignTypography *a, const UmiDesignTypography *b)
{
    return strcmp(a->family, b->family) == 0 &&
        a->size == b->size &&
        a->weight == b->weight &&
        a->line_height == b->line_height &&
        a->letter_spacing == b->letter_spacing;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignTypographyTransferTails(UmiDesignTypography *value)
{
    (void)value;
    {
        size_t used = strlen(value->family) + 1U;
        memset(value->family + used, 0xa5, sizeof(value->family) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignTypographyTransferMalformed(const UmiDesignTypography *sample)
{
    (void)sample;
    {
        UmiDesignTypography invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.family, 'x', sizeof(invalid.family));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_design_typography_valid(&invalid)) ||
            umi_design_typography_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated family was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignTypographyTransferCases, UmiDesignTypography,
    umi_design_typography_archive_encode, umi_design_typography_archive_decode,
    UmiDesignTypographyTransferEqual, UmiDesignTypographyTransferTails, UmiDesignTypographyTransferMalformed)

int main(void) { UmiDesignTypography t; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_typography_init(&t,"Sans",14.0,500U,1.4)!=UMI_STATUS_OK)return 1;
    if (UmiDesignTypographyTransferCases(&t) != 0) return 1;
 return umi_design_typography_valid(&t)?0:2; }
