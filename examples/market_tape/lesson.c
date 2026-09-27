/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/market_tape/lesson.c
 *
 * PURPOSE:
 *   Build one-second bars from canonical observations using the public C API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading/market_tape.h"
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    UmiMarketTape *tape = NULL;
    UmiMarketTapeConfig config = UmiMarketTapeConfigDefault();
    UmiMarketTapeSnapshot *view = calloc(1U, sizeof(*view));
    UmiInstrument instrument = {.instrument_id = {"PRACTICE_ACCOUNT"}, .symbol = "WORKSHOP",
        .venue = "PRACTICE", .currency = {"GBP"}, .multiplier = 1.0};
    UmiStatus status;
    if (view == NULL) return 1;
    status = UmiMarketTapeCreate(&config, &tape);
    if (status == UMI_STATUS_OK) status = UmiMarketTapeRegister(tape, &instrument);
    if (status == UMI_STATUS_OK) status = UmiMarketTapeBegin(tape, &instrument.instrument_id, 1U, 1U);
    const double prices[] = {100.0, 101.0, 99.0, 100.5};
    for (size_t i = 0U; i < 4U && status == UMI_STATUS_OK; ++i) {
        UmiMarketTapePacket packet = {0};
        packet.kind = UMI_MARKET_TAPE_TRADE;
        packet.generation = 1U; packet.sequence = (uint64_t)i + 1U;
        packet.receivedTimeMs = 1000 + (int64_t)i * 200;
        packet.value.trade = (UmiTradeTick){instrument, prices[i], 2.0, packet.receivedTimeMs};
        status = UmiMarketTapeApply(tape, &packet, NULL);
    }
    if (status == UMI_STATUS_OK) status = UmiMarketTapeRead(tape, 1800, view);
    if (status == UMI_STATUS_OK && view->barCount == 1U) {
        UmiBar bar = view->bars[0];
        printf("WORKSHOP: open %.2f, high %.2f, low %.2f, close %.2f, volume %.0f\n",
            bar.open, bar.high, bar.low, bar.close, bar.volume);
        puts("Four trades share one bar. The missing quote is not a zero price.\nPractice complete. Memory only; no order was created.");
    } else status = UMI_STATUS_INTERNAL_ERROR;
    UmiMarketTapeDestroy(tape);
    free(view);
    return status == UMI_STATUS_OK ? 0 : 1;
}
