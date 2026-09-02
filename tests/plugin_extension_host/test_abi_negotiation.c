/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_abi_negotiation.c
 *
 * PURPOSE:
 *   Exercise negotiate ABI versions and feature flags without loading incompatible extensions.
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
#include "umicom/plugin/extension_host/abi_negotiation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostAbiNegotiation n; umi_plugin_extension_host_abi_negotiation_init(&n); n.host_version=3U; n.minimum_version=2U; n.maximum_version=4U; n.required_flags=UINT64_C(3); n.optional_flags=UINT64_C(4); n.supported_flags=UINT64_C(7); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_abi_negotiation_evaluate(&n)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_plugin_extension_host_abi_negotiation_compatible(&n)||n.negotiated_flags!=UINT64_C(7)) return 2; return 0; }
