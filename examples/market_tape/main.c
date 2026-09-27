/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/market_tape/main.c
 *
 * PURPOSE:
 *   Inspect deterministic market observations without contacting a provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading/market_tape_practice.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int Run(bool quiet)
{
    UmiMarketTapePractice *practice = NULL;
    UmiMarketTapeSnapshot *view = calloc(1U, sizeof(*view));
    UmiMarketTapeRejection reason;
    if (view == NULL) return 1;
    UmiStatus status = UmiMarketTapePracticeCreate(&practice);
    for (unsigned i = 0U; i < 24U && status == UMI_STATUS_OK; ++i)
        status = UmiMarketTapePracticeAct(practice, UMI_MARKET_PRACTICE_NEXT, &reason);
    if (status == UMI_STATUS_OK) status = UmiMarketTapePracticeRead(practice, view);
    if (status == UMI_STATUS_OK && !quiet) {
        printf("PRACTICE ONLY | %s | %zu retained trades | %zu bars\n",
            view->rows[view->selectedIndex].instrument.symbol, view->tradeCount, view->barCount);
        printf("Last sequence: %" PRIu64 "\n", view->rows[0].lastSequence);
    }
    if (status == UMI_STATUS_OK) {
        status = UmiMarketTapePracticeAct(practice, UMI_MARKET_PRACTICE_GAP, &reason);
        if (status == UMI_STATUS_INVALID_STATE && reason == UMI_MARKET_TAPE_SEQUENCE_GAP) {
            status = UmiMarketTapePracticeRead(practice, view);
            if (status == UMI_STATUS_OK && (view->rows[0].state != UMI_MARKET_TAPE_GAP || view->tradeCount != 24U))
                status = UMI_STATUS_INTERNAL_ERROR;
        } else status = UMI_STATUS_INTERNAL_ERROR;
    }
    if (status == UMI_STATUS_OK && !quiet)
        puts("The skipped sequence latched a gap. Its price was not applied.");
    if (status == UMI_STATUS_OK)
        status = UmiMarketTapePracticeAct(practice, UMI_MARKET_PRACTICE_NEW_EPOCH, &reason);
    if (status == UMI_STATUS_OK) status = UmiMarketTapePracticeRead(practice, view);
    if (status == UMI_STATUS_OK && (view->tradeCount != 0U || view->rows[0].generation != 2U))
        status = UMI_STATUS_INTERNAL_ERROR;
    if (status == UMI_STATUS_OK)
        puts(quiet ? "Native market-tape self-check passed. Memory only." :
            "A new epoch cleared the selected history; it did not repair the missing tick.\nPractice complete. No file, database, broker or network was opened.");
    else fprintf(stderr, "Market tape: %s\n", umi_status_text(status));
    UmiMarketTapePracticeDestroy(practice);
    free(view);
    return status == UMI_STATUS_OK ? 0 : 1;
}
int main(int argc, char **argv)
{
    if (argc == 1 || (argc == 2 && strcmp(argv[1], "demo") == 0)) return Run(false);
    if (argc == 2 && strcmp(argv[1], "--self-test") == 0) return Run(true);
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        puts("umicom-market-tape [demo | --self-test | --help]\nNo live provider is connected."); return 0;
    }
    fputs("Use --help for supported commands.\n", stderr);
    return 2;
}
