/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_button_spec.c
 *
 * PURPOSE:
 *   Verify the semantic button spec contract.
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
#include "umicom/ui/design/button_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/button_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignButtonSpecTransferEqual(const UmiDesignButtonSpec *a, const UmiDesignButtonSpec *b)
{
    return strcmp(a->label, b->label) == 0 &&
        a->role == b->role &&
        a->density == b->density &&
        a->icon_only == b->icon_only &&
        a->destructive == b->destructive;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignButtonSpecTransferTails(UmiDesignButtonSpec *value)
{
    (void)value;
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignButtonSpecTransferMalformed(const UmiDesignButtonSpec *sample)
{
    (void)sample;
    {
        UmiDesignButtonSpec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_design_button_spec_valid(&invalid)) ||
            umi_design_button_spec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignButtonSpecTransferCases, UmiDesignButtonSpec,
    umi_design_button_spec_archive_encode, umi_design_button_spec_archive_decode,
    UmiDesignButtonSpecTransferEqual, UmiDesignButtonSpecTransferTails, UmiDesignButtonSpecTransferMalformed)

int main(void){UmiDesignButtonSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_button_spec_init(&s,"Save",UMI_DESIGN_ROLE_PRIMARY,UMI_DESIGN_DENSITY_STANDARD,0,0)!=UMI_STATUS_OK)return 1;
    if (UmiDesignButtonSpecTransferCases(&s) != 0) return 1;
return s.label[0]=='S'?0:2;}
