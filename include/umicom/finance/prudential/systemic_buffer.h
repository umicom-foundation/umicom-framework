/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/systemic_buffer.h
 *
 * PURPOSE:
 *   Calculate a systemic capital buffer amount from RWA and configured rate.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_SYSTEMIC_BUFFER_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_SYSTEMIC_BUFFER_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialSystemicBuffer { char buffer_id[UMI_PRU_ID_CAPACITY]; double required_ratio; double rwa; double required_amount; } UmiPrudentialSystemicBuffer;
/* Calculate a named capital buffer amount from its required ratio and RWA. */
UmiStatus umi_pru_systemic_buffer_calculate(UmiPrudentialSystemicBuffer *result, const char *buffer_id, double required_ratio, double rwa);

#ifdef __cplusplus
}
#endif

#endif
