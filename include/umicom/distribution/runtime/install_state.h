/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/install_state.h
 *
 * PURPOSE:
 *   installed application version, channel and health state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_INSTALL_STATE_H
#define UMICOM_DISTRIBUTION_RUNTIME_INSTALL_STATE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr install state data shared with callers of this public contract.
 */
typedef struct UmiDrInstallState { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; UmiDrVersion version; UmiDrChannelKind channel; UmiDrInstallScope scope; bool healthy; } UmiDrInstallState;
/**
 * Initialise dr install state from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_install_state_init(UmiDrInstallState *value);
/**
 * Check that dr install state satisfies its contract before another service relies on it.
 */
bool umi_dr_install_state_valid(const UmiDrInstallState *value);
/**
 * Provide the dr install state fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_install_state_fingerprint(const UmiDrInstallState *value);

#ifdef __cplusplus
}
#endif
#endif
