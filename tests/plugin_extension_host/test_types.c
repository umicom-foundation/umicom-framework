/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_types.c
 *
 * PURPOSE:
 *   Exercise define shared stable states, decisions, capacities and evidence helpers for the isolated plug-in extension host.
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
#include <string.h>
#include "umicom/plugin/extension_host/types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_plugin_extension_host_text_valid("plugin.sample", 32U)) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_plugin_extension_host_hash_text("plugin.sample") == 0U) return 2;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(umi_plugin_extension_host_decision_text(UMI_PLUGIN_EXTENSION_HOST_ALLOW), "allow") != 0) return 3;
    return 0;
}
