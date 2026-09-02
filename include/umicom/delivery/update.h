/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/update.h
 *
 * PURPOSE:
 *   Compare installed and available generations and produce a safe update decision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Updates are based on immutable generation numbers so the application can explain why a newer release is or is not eligible.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_UPDATE_H
#define INCLUDE_UMICOM_DELIVERY_UPDATE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the update decide operation used by this module and its client applications.
 */
UmiUpdateDecision umi_update_decide(uint64_t installed_generation,
                                    uint64_t available_generation,
                                    int compatible,
                                    int policy_allows);

#ifdef __cplusplus
}
#endif

#endif
