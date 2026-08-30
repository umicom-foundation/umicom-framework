/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/event_contract.h
 *
 * PURPOSE:
 *   semantic user-event support requirements independent of native toolkit event classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_EVENT_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_EVENT_CONTRACT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcEventContract { uint64_t required_families; bool ordered; bool cancellable; } UmiFcEventContract;
bool umi_fc_event_contract_validate(const UmiFcEventContract *item);

#ifdef __cplusplus
}
#endif
#endif
