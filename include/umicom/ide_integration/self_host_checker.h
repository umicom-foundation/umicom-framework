/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/self_host_checker.h
 *
 * PURPOSE:
 *   Verify self-hosting manifest closure against actual registered surfaces,
 *   runtime commands and the host Framework capability catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_SELF_HOST_CHECKER_H
#define UMICOM_IDE_INTEGRATION_SELF_HOST_CHECKER_H

#include "umicom/ide_integration/self_host_manifest.h"
#include "umicom/ide_integration/surface.h"
#include "umicom/runtime/command_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_IDE_SELF_HOST_MISSING_CAPACITY 32U

typedef int (*UmiIdeCapabilityLookup)(
    void *user_data,
    const char *capability_id);

typedef struct UmiIdeSelfHostCheckReport {
    char missing_surfaces[UMI_IDE_SELF_HOST_MISSING_CAPACITY]
                         [UMI_IDE_INTEGRATION_ID_CAPACITY];
    char missing_commands[UMI_IDE_SELF_HOST_MISSING_CAPACITY]
                         [UMI_IDE_INTEGRATION_ID_CAPACITY];
    char missing_capabilities[UMI_IDE_SELF_HOST_MISSING_CAPACITY]
                             [UMI_IDE_INTEGRATION_ID_CAPACITY];
    size_t missing_surface_count;
    size_t missing_command_count;
    size_t missing_capability_count;
    uint64_t revision;
    int ready;
} UmiIdeSelfHostCheckReport;

UmiStatus umi_ide_self_host_check(
    const UmiIdeSelfHostManifest *manifest,
    const UmiIdeSurfaceRegistry *surfaces,
    const UmiCommandRegistry *commands,
    UmiIdeCapabilityLookup capability_lookup,
    void *capability_user_data,
    UmiIdeSelfHostCheckReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
