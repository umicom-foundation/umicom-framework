/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_alert_spec.c
 *
 * PURPOSE:
 *   Verify the semantic alert spec contract.
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
#include "umicom/ui/design/alert_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/alert_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignAlertSpecTransferEqual(const UmiDesignAlertSpec *a, const UmiDesignAlertSpec *b)
{
    return a->severity == b->severity &&
        strcmp(a->message, b->message) == 0 &&
        a->dismissible == b->dismissible &&
        a->actionable == b->actionable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignAlertSpecTransferTails(UmiDesignAlertSpec *value)
{
    (void)value;
    {
        size_t used = strlen(value->message) + 1U;
        memset(value->message + used, 0xa5, sizeof(value->message) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignAlertSpecTransferMalformed(const UmiDesignAlertSpec *sample)
{
    (void)sample;
    {
        UmiDesignAlertSpec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.message, 'x', sizeof(invalid.message));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_design_alert_spec_valid(&invalid)) ||
            umi_design_alert_spec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated message was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignAlertSpecTransferCases, UmiDesignAlertSpec,
    umi_design_alert_spec_archive_encode, umi_design_alert_spec_archive_decode,
    UmiDesignAlertSpecTransferEqual, UmiDesignAlertSpecTransferTails, UmiDesignAlertSpecTransferMalformed)

int main(void){UmiDesignAlertSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_alert_spec_init(&s,UMI_UI_SEVERITY_WARNING,"Connection degraded",1,1)!=UMI_STATUS_OK)return 1;
    if (UmiDesignAlertSpecTransferCases(&s) != 0) return 1;
return s.actionable?0:2;}
