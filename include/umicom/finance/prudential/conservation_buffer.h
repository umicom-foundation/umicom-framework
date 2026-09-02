/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/conservation_buffer.h
 *
 * PURPOSE:
 *   Calculate a capital conservation buffer amount from RWA.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CONSERVATION_BUFFER_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CONSERVATION_BUFFER_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential conservation buffer data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialConservationBuffer { char buffer_id[UMI_PRU_ID_CAPACITY]; double required_ratio; double rwa; double required_amount; } UmiPrudentialConservationBuffer;
/* Calculate a named capital buffer amount from its required ratio and RWA. */
UmiStatus umi_pru_conservation_buffer_calculate(UmiPrudentialConservationBuffer *result, const char *buffer_id, double required_ratio, double rwa);

#ifdef __cplusplus
}
#endif

#endif
