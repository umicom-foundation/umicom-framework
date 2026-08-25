/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/command_contract.h
 *
 * PURPOSE:
 *   stable command exposure expectations independent of frontend toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFcCommandContract { size_t required_commands; uint64_t command_fingerprint; bool all_have_stable_ids; } UmiFcCommandContract;
bool umi_fc_command_contract_validate(const UmiFcCommandContract *item);

#ifdef __cplusplus
}
#endif
#endif
