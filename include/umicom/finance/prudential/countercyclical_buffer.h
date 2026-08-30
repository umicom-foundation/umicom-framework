/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/countercyclical_buffer.h
 *
 * PURPOSE:
 *   Calculate a countercyclical capital buffer amount from RWA and configured rate.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_COUNTERCYCLICAL_BUFFER_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_COUNTERCYCLICAL_BUFFER_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialCountercyclicalBuffer { char buffer_id[UMI_PRU_ID_CAPACITY]; double required_ratio; double rwa; double required_amount; } UmiPrudentialCountercyclicalBuffer;
/* Calculate a named capital buffer amount from its required ratio and RWA. */
UmiStatus umi_pru_countercyclical_buffer_calculate(UmiPrudentialCountercyclicalBuffer *result, const char *buffer_id, double required_ratio, double rwa);

#ifdef __cplusplus
}
#endif

#endif
