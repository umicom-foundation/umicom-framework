/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/install_state.h
 *
 * PURPOSE:
 *   installed application version, channel and health state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiDrInstallState { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; UmiDrVersion version; UmiDrChannelKind channel; UmiDrInstallScope scope; bool healthy; } UmiDrInstallState;
void umi_dr_install_state_init(UmiDrInstallState *value);
bool umi_dr_install_state_valid(const UmiDrInstallState *value);
uint64_t umi_dr_install_state_fingerprint(const UmiDrInstallState *value);

#ifdef __cplusplus
}
#endif
#endif
