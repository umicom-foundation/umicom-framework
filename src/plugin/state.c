/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/state.c
 *
 * PURPOSE:
 *   Implement explicit plug-in lifecycle transition rules and stable text
 *   labels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/state.h"
/*
 * Check that plugin state transition satisfies its contract before another service relies
 * on it.
 */
int umi_plugin_state_transition_valid(UmiPluginState from, UmiPluginState to)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (to == UMI_PLUGIN_FAILED || to == UMI_PLUGIN_DISABLED) return 1;
    /* Select the behaviour associated with the requested command or state value. */
    switch (from) {
        case UMI_PLUGIN_DISCOVERED: return to == UMI_PLUGIN_VALIDATED;
        case UMI_PLUGIN_VALIDATED: return to == UMI_PLUGIN_LOADED;
        case UMI_PLUGIN_LOADED: return to == UMI_PLUGIN_CONFIGURED || to == UMI_PLUGIN_STOPPED;
        case UMI_PLUGIN_CONFIGURED: return to == UMI_PLUGIN_STARTED || to == UMI_PLUGIN_STOPPED;
        case UMI_PLUGIN_STARTED: return to == UMI_PLUGIN_QUIESCED || to == UMI_PLUGIN_STOPPED;
        case UMI_PLUGIN_QUIESCED: return to == UMI_PLUGIN_STARTED || to == UMI_PLUGIN_STOPPED;
        case UMI_PLUGIN_STOPPED: return to == UMI_PLUGIN_LOADED;
        case UMI_PLUGIN_FAILED: return to == UMI_PLUGIN_STOPPED;
        case UMI_PLUGIN_DISABLED: return to == UMI_PLUGIN_DISCOVERED;
        default: return 0;
    }
}
/* Provide the plugin state text operation used by this module and its client applications. */
const char *umi_plugin_state_text(UmiPluginState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_PLUGIN_DISCOVERED: return "discovered";
        case UMI_PLUGIN_VALIDATED: return "validated";
        case UMI_PLUGIN_LOADED: return "loaded";
        case UMI_PLUGIN_CONFIGURED: return "configured";
        case UMI_PLUGIN_STARTED: return "started";
        case UMI_PLUGIN_QUIESCED: return "quiesced";
        case UMI_PLUGIN_STOPPED: return "stopped";
        case UMI_PLUGIN_FAILED: return "failed";
        case UMI_PLUGIN_DISABLED: return "disabled";
        default: return "unknown";
    }
}
