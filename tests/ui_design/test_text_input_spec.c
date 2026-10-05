/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_text_input_spec.c
 *
 * PURPOSE:
 *   Verify the semantic text input spec contract.
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
#include "umicom/ui/design/text_input_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/text_input_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignTextInputSpecTransferEqual(const UmiDesignTextInputSpec *a, const UmiDesignTextInputSpec *b)
{
    return strcmp(a->placeholder, b->placeholder) == 0 &&
        a->max_length == b->max_length &&
        a->password == b->password &&
        a->search == b->search &&
        a->multiline == b->multiline;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignTextInputSpecTransferTails(UmiDesignTextInputSpec *value)
{
    (void)value;
    {
        size_t used = strlen(value->placeholder) + 1U;
        memset(value->placeholder + used, 0xa5, sizeof(value->placeholder) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignTextInputSpecTransferMalformed(const UmiDesignTextInputSpec *sample)
{
    (void)sample;
    {
        UmiDesignTextInputSpec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.placeholder, 'x', sizeof(invalid.placeholder));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_design_text_input_spec_valid(&invalid)) ||
            umi_design_text_input_spec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated placeholder was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignTextInputSpecTransferCases, UmiDesignTextInputSpec,
    umi_design_text_input_spec_archive_encode, umi_design_text_input_spec_archive_decode,
    UmiDesignTextInputSpecTransferEqual, UmiDesignTextInputSpecTransferTails, UmiDesignTextInputSpecTransferMalformed)

int main(void){UmiDesignTextInputSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_text_input_spec_init(&s,"Search…",256U,0,1,0)!=UMI_STATUS_OK)return 1;
    if (UmiDesignTextInputSpecTransferCases(&s) != 0) return 1;
return s.search?0:2;}
