/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/context_contract.h
 *
 * PURPOSE:
 *   typed context-channel requirements for linked cross-application surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the fc context contract data shared with callers of this public contract.
 */
typedef struct UmiFcContextContract { uint64_t required_types; bool bidirectional; bool accessible_label; } UmiFcContextContract;
/**
 * Check that fc context contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_context_contract_validate(const UmiFcContextContract *item);

#ifdef __cplusplus
}
#endif
#endif
