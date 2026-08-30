/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/self_host_manifest.h
 *
 * PURPOSE:
 *   Declare the Framework capabilities, commands and surfaces required before an
 *   IDE composition can be considered self-hosting capable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_SELF_HOST_MANIFEST_H
#define UMICOM_IDE_INTEGRATION_SELF_HOST_MANIFEST_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdeSelfHostManifest {
    const char *const *required_surface_ids;
    size_t required_surface_count;
    const char *const *required_command_ids;
    size_t required_command_count;
    const char *const *required_capabilities;
    size_t required_capability_count;
} UmiIdeSelfHostManifest;

const UmiIdeSelfHostManifest *umi_ide_self_host_manifest(void);

UmiStatus umi_ide_self_host_manifest_validate(
    const UmiIdeSelfHostManifest *manifest);

#ifdef __cplusplus
}
#endif
#endif
