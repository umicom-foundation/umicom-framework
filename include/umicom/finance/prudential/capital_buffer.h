/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/capital_buffer.h
 *
 * PURPOSE:
 *   Represent a capital buffer expressed as a required ratio and amount.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_BUFFER_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_BUFFER_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialCapitalBuffer { char buffer_id[UMI_PRU_ID_CAPACITY]; double required_ratio; double rwa; double required_amount; } UmiPrudentialCapitalBuffer;
/* Calculate a named capital buffer amount from its required ratio and RWA. */
UmiStatus umi_pru_capital_buffer_calculate(UmiPrudentialCapitalBuffer *result, const char *buffer_id, double required_ratio, double rwa);

#ifdef __cplusplus
}
#endif

#endif
