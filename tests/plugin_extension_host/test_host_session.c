/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_host_session.c
 *
 * PURPOSE:
 *   Exercise track one extension-host session through connect, ready, draining and closed states.
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
#include "umicom/plugin/extension_host/host_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostHostSession s; umi_plugin_extension_host_host_session_init(&s,"s1"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_host_session_transition(&s,UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_CONNECTING)!=UMI_STATUS_OK||umi_plugin_extension_host_host_session_transition(&s,UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_READY)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_plugin_extension_host_host_session_ready(&s)) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_host_session_transition(&s,UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_DISCONNECTED)==UMI_STATUS_OK) return 3; return 0; }
