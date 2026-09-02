/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_quarantine_policy.c
 *
 * PURPOSE:
 *   Exercise determine when repeated crashes, trust failures or signature failures require quarantine.
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
#include "umicom/plugin/extension_host/quarantine_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostQuarantinePolicy p; umi_plugin_extension_host_quarantine_policy_init(&p); /* Apply this branch only when its contract condition is satisfied. */ if(umi_plugin_extension_host_quarantine_policy_should_quarantine(&p,1U,0U,0U,0U)) return 1; /* Apply this branch only when its contract condition is satisfied. */ if(!umi_plugin_extension_host_quarantine_policy_should_quarantine(&p,3U,0U,0U,0U)) return 2; return 0; }
