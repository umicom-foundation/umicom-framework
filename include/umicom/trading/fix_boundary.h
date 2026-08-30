/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/fix_boundary.h
 *
 * PURPOSE:
 *   Validate minimal FIX-style sequence and session identifiers without binding to a specific FIX engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates fix boundary behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_FIX_BOUNDARY_H
#define INCLUDE_UMICOM_TRADING_FIX_BOUNDARY_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFixSessionInfo { char sender_comp_id[32]; char target_comp_id[32]; uint64_t next_out_sequence; uint64_t next_in_sequence; } UmiFixSessionInfo;
int umi_fix_session_info_valid(const UmiFixSessionInfo *session);
#ifdef __cplusplus
}
#endif
#endif
