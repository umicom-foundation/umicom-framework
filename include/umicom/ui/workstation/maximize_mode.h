/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/maximize_mode.h
 *
 * PURPOSE:
 *   Define reversible panel maximisation state independent of persistent layout geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_MAXIMIZE_MODE_H
#define UMICOM_UI_WORKSTATION_MAXIMIZE_MODE_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws maximize mode data shared with callers of this public contract.
 */
typedef struct UmiWsMaximizeMode {
    char surface_id[UMI_UI_ID_CAPACITY];
    bool active;
} UmiWsMaximizeMode;

/**
 * Provide the ws maximize mode activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_maximize_mode_activate(UmiWsMaximizeMode *mode, const char *surface_id);
/**
 * Provide the ws maximize mode restore operation used by this module and its client
 * applications.
 */
void umi_ws_maximize_mode_restore(UmiWsMaximizeMode *mode);
/**
 * Provide the ws maximize mode matches operation used by this module and its client
 * applications.
 */
bool umi_ws_maximize_mode_matches(const UmiWsMaximizeMode *mode, const char *surface_id);

#ifdef __cplusplus
}
#endif

#endif
