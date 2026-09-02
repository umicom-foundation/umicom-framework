/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_manifest_validation.c
 *
 * PURPOSE:
 *   Exercise validate extension manifest evidence before installation or activation.
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
#include "umicom/plugin/extension_host/manifest_validation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostManifestValidation r; umi_plugin_extension_host_manifest_validation_init(&r); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_manifest_validation_record(&r,1,1,1,1,1,1)!=UMI_STATUS_OK||!umi_plugin_extension_host_manifest_validation_ready(&r)) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_manifest_validation_record(&r,1,0,1,1,1,1)!=UMI_STATUS_OK||umi_plugin_extension_host_manifest_validation_ready(&r)) return 2; return 0; }
