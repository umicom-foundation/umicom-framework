/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/install_state.h
 *
 * PURPOSE:
 *   Record the currently installed application version and active generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Update and rollback decisions need one authoritative view of what is installed and whether it is healthy.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_INSTALL_STATE_H
#define INCLUDE_UMICOM_DELIVERY_INSTALL_STATE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiInstallState {
    char application_id[UMI_DELIVERY_ID_CAPACITY];
    char version[UMI_DELIVERY_VERSION_CAPACITY];
    uint64_t generation;
    char install_root[UMI_DELIVERY_PATH_CAPACITY];
    int healthy;
} UmiInstallState;

UmiStatus umi_install_state_init(UmiInstallState *state,
                                 const char *application_id,
                                 const char *version,
                                 uint64_t generation,
                                 const char *install_root);

#ifdef __cplusplus
}
#endif

#endif
