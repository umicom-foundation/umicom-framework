/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/market_tape_practice.h
 *
 * PURPOSE:
 *   Drive a labelled, deterministic two-instrument practice feed without I/O.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_MARKET_TAPE_PRACTICE_H
#define UMICOM_TRADING_MARKET_TAPE_PRACTICE_H
#include "umicom/trading/market_tape.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiMarketTapePractice UmiMarketTapePractice;
typedef enum UmiMarketTapePracticeAction {
    UMI_MARKET_PRACTICE_NEXT = 1,
    UMI_MARKET_PRACTICE_GAP,
    UMI_MARKET_PRACTICE_AGE,
    UMI_MARKET_PRACTICE_DISCONNECT,
    UMI_MARKET_PRACTICE_NEW_EPOCH
} UmiMarketTapePracticeAction;
/* Single-owner practice controller. No clock, files, broker or network.
 * Its time advances ONLY through explicit actions, not elapsed wall time. */
UmiStatus UmiMarketTapePracticeCreate(UmiMarketTapePractice **outPractice);
void UmiMarketTapePracticeDestroy(UmiMarketTapePractice *practice);
UmiStatus UmiMarketTapePracticeSelect(UmiMarketTapePractice *practice, size_t index);
UmiStatus UmiMarketTapePracticeRead(UmiMarketTapePractice *practice, UmiMarketTapeSnapshot *outSnapshot);
UmiStatus UmiMarketTapePracticeAct(UmiMarketTapePractice *practice,
    UmiMarketTapePracticeAction action, UmiMarketTapeRejection *outReason);
#ifdef __cplusplus
}
#endif
#endif
