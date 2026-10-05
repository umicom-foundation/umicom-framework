/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_dock_transaction.c
 *
 * PURPOSE:
 *   Exercise dock transaction behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/ui/mosaic/dock_transaction.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/mosaic/dock_transaction.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiMosaicDockTransactionTransferEqual(const UmiUiMosaicDockTransaction *a, const UmiUiMosaicDockTransaction *b)
{
    return strcmp(a->source_id, b->source_id) == 0 &&
        strcmp(a->target_id, b->target_id) == 0 &&
        a->zone == b->zone &&
        a->sequence == b->sequence &&
        a->allowed == b->allowed;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiMosaicDockTransactionTransferTails(UmiUiMosaicDockTransaction *value)
{
    (void)value;
    {
        size_t used = strlen(value->source_id) + 1U;
        memset(value->source_id + used, 0xa5, sizeof(value->source_id) - used);
    }
    {
        size_t used = strlen(value->target_id) + 1U;
        memset(value->target_id + used, 0xa5, sizeof(value->target_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiMosaicDockTransactionTransferMalformed(const UmiUiMosaicDockTransaction *sample)
{
    (void)sample;
    {
        UmiUiMosaicDockTransaction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_id, 'x', sizeof(invalid.source_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_dock_transaction_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_dock_transaction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiMosaicDockTransaction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_id, 'x', sizeof(invalid.target_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_dock_transaction_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_dock_transaction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiMosaicDockTransactionTransferCases, UmiUiMosaicDockTransaction,
    umi_ui_mosaic_dock_transaction_archive_encode, umi_ui_mosaic_dock_transaction_archive_decode,
    UmiUiMosaicDockTransactionTransferEqual, UmiUiMosaicDockTransactionTransferTails, UmiUiMosaicDockTransactionTransferMalformed)

int main(void) {
    UmiUiMosaicDockTransaction value;
    umi_ui_mosaic_dock_transaction_init(&value);
    CHECK(umi_ui_mosaic_dock_transaction_set(&value, "panel.source", "panel.target", UMI_UI_MOSAIC_DOCK_CENTRE) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_dock_transaction_validate(&value) == UMI_STATUS_OK);
    if (UmiUiMosaicDockTransactionTransferCases(&value) != 0) return 1;

    CHECK(umi_ui_mosaic_dock_transaction_is_centre(&value) == 1);
    return 0;
}
