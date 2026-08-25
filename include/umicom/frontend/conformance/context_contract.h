/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/context_contract.h
 *
 * PURPOSE:
 *   typed context-channel requirements for linked cross-application surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_CONTEXT_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_CONTEXT_CONTRACT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcContextContract { uint64_t required_types; bool bidirectional; bool accessible_label; } UmiFcContextContract;
bool umi_fc_context_contract_validate(const UmiFcContextContract *item);

#ifdef __cplusplus
}
#endif
#endif
