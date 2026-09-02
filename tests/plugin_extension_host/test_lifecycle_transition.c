/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_lifecycle_transition.c
 *
 * PURPOSE:
 *   Exercise validate explicit extension lifecycle transitions.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/lifecycle_transition.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_lifecycle_transition_validate(UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_DISCOVERED,UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_VERIFIED)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_lifecycle_transition_allowed(UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_DISCOVERED,UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING)) return 2; return 0; }
