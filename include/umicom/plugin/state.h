/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/state.h
 *
 * PURPOSE:
 *   Define validated plug-in lifecycle states and transitions shared by
 *   registries, loaders, hosts and Studio views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_STATE_H
#define UMICOM_PLUGIN_STATE_H

#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named plugin state values accepted by this public contract.
 */
typedef enum UmiPluginState {
    UMI_PLUGIN_DISCOVERED = 0,
    UMI_PLUGIN_VALIDATED = 1,
    UMI_PLUGIN_LOADED = 2,
    UMI_PLUGIN_CONFIGURED = 3,
    UMI_PLUGIN_STARTED = 4,
    UMI_PLUGIN_QUIESCED = 5,
    UMI_PLUGIN_STOPPED = 6,
    UMI_PLUGIN_FAILED = 7,
    UMI_PLUGIN_DISABLED = 8
} UmiPluginState;
/**
 * Check that plugin state transition satisfies its contract before another service relies
 * on it.
 */
int umi_plugin_state_transition_valid(UmiPluginState from, UmiPluginState to);
/**
 * Provide the plugin state text operation used by this module and its client applications.
 */
const char *umi_plugin_state_text(UmiPluginState state);
#ifdef __cplusplus
}
#endif

#endif
