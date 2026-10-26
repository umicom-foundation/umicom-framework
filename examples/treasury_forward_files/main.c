/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/treasury_forward_files/main.c
 *
 * PURPOSE:
 *   Demonstrate explicit new-file persistence through the installed treasury SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/open_tms/fx_forward_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: treasury-forward-files <new-absolute-file-path>\n");
        return 2;
    }
    UmiOpenTmsFxForwardBook *book = malloc(sizeof *book), *loaded = malloc(sizeof *loaded);
    if (book == NULL || loaded == NULL) {
        free(loaded); free(book); return 1;
    }
    umi_open_tms_fx_forward_book_init(book);
    UmiOpenTmsFxForward trade = {0};
    strcpy(trade.tradeId, "synthetic-example"); strcpy(trade.accountId, "practice");
    strcpy(trade.counterpartyId, "example-only"); strcpy(trade.baseCurrency, "GBP"); strcpy(trade.quoteCurrency, "USD");
    trade.baseNotional = 1000.0; trade.forwardRate = 1.25; trade.marketForwardRate = 0.0;
    trade.version = 1U; trade.state = UMI_OPEN_TMS_FX_FORWARD_DRAFT;
    UmiOutputFileSnapshot receipt = {0};
    UmiStatus status = umi_open_tms_fx_forward_book_add(book, &trade);
    if (status == UMI_STATUS_OK) status = umi_open_tms_fx_forward_book_save_new(argv[1], book, &receipt);
    if (status == UMI_STATUS_OK) status = umi_open_tms_fx_forward_book_load(argv[1], loaded);
    if (status == UMI_STATUS_OK && (loaded->count != 1U || loaded->revision != book->revision ||
        strcmp(loaded->trades[0].tradeId, trade.tradeId) != 0 ||
        loaded->trades[0].baseNotional != trade.baseNotional ||
        loaded->trades[0].forwardRate != trade.forwardRate)) status = UMI_STATUS_INTERNAL_ERROR;
    if (status == UMI_STATUS_OK)
        puts("Saved and reopened one synthetic local record. No financial action occurred.");
    else fprintf(stderr, "Archive request refused: %s. Existing or partial files are retained.\n", umi_status_text(status));
    free(loaded); free(book);
    return status == UMI_STATUS_OK ? 0 : 1;
}
