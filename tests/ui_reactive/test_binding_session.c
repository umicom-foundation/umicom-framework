/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_binding_session.c
 *
 * PURPOSE:
 *   Exercise the binding session reactive UI contract.
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
#include "umicom/ui/reactive/binding_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/binding_session.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveBindingSessionTransferEqual(const UmiUiReactiveBindingSession *a, const UmiUiReactiveBindingSession *b)
{
    return strcmp(a->session_id, b->session_id) == 0 &&
        a->active == b->active &&
        a->revision == b->revision &&
        a->propagations == b->propagations;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveBindingSessionTransferTails(UmiUiReactiveBindingSession *value)
{
    (void)value;
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveBindingSessionTransferMalformed(const UmiUiReactiveBindingSession *sample)
{
    (void)sample;
    {
        UmiUiReactiveBindingSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.session_id, 'x', sizeof(invalid.session_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_binding_session_valid(&invalid)) ||
            umi_ui_reactive_binding_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated session_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveBindingSessionTransferCases, UmiUiReactiveBindingSession,
    umi_ui_reactive_binding_session_archive_encode, umi_ui_reactive_binding_session_archive_decode,
    UmiUiReactiveBindingSessionTransferEqual, UmiUiReactiveBindingSessionTransferTails, UmiUiReactiveBindingSessionTransferMalformed)

int main(void) { UmiUiReactiveBindingSession item; umi_ui_reactive_binding_session_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveBindingSession populated = item;
    (void)snprintf(populated.session_id, sizeof(populated.session_id), "field-0");
    populated.active = true;
    populated.revision = (uint64_t)4;
    populated.propagations = (size_t)5;
    if (UmiUiReactiveBindingSessionTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_binding_session_valid(&item) ? 0 : 1; }
