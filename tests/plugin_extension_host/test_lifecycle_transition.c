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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/lifecycle_transition.h"
int main(void) { if(umi_plugin_extension_host_lifecycle_transition_validate(UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_DISCOVERED,UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_VERIFIED)!=UMI_STATUS_OK) return 1; if(umi_plugin_extension_host_lifecycle_transition_allowed(UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_DISCOVERED,UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING)) return 2; return 0; }
