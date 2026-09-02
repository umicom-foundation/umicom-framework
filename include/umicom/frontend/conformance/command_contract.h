/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/command_contract.h
 *
 * PURPOSE:
 *   stable command exposure expectations independent of frontend toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_COMMAND_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_COMMAND_CONTRACT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc command contract data shared with callers of this public contract.
 */
typedef struct UmiFcCommandContract { size_t required_commands; uint64_t command_fingerprint; bool all_have_stable_ids; } UmiFcCommandContract;
/**
 * Check that fc command contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_command_contract_validate(const UmiFcCommandContract *item);

#ifdef __cplusplus
}
#endif
#endif
