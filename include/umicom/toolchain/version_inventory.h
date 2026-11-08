/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/version_inventory.h
 *
 * PURPOSE:
 *   Save bounded tool discovery evidence without claiming compiler or project qualification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_VERSION_INVENTORY_H
#define UMICOM_TOOLCHAIN_VERSION_INVENTORY_H
#include "umicom/toolchain/discovery.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TOOLCHAIN_VERSION_INVENTORY_CAPACITY (1024U * 1024U)
/* Serialize an already captured version-only discovery report as schema-1 JSON.
 * This function does not discover tools, execute processes or access storage.
 * Pass the status returned by discovery, including an incomplete result.
 * All emitted fields must be terminated UTF-8. Compile/link/runtime/C23 pass
 * flags and c23_capable must be zero: the caller must have used discovery's
 * skip_compile_probe option. The report cannot independently prove that fact.
 * Include missing tools explicitly. Counts are discovery observations; required
 * tools may include the logical requirement for one usable C compiler vendor.
 * Output must not alias the report. On failure clear a writable output buffer
 * and set out_size to zero. Size excludes the terminator. No binary hashes,
 * source revisions, cache identity, environment or session transcript are claimed. */
UmiStatus umi_toolchain_version_inventory_format_sized(
    const UmiToolchainDiscoveryReport *report, size_t report_size,
    UmiStatus discovery_status, char *out_json, size_t capacity, size_t *out_size);
#define umi_toolchain_version_inventory_format(report, status, json, capacity, size) \
    umi_toolchain_version_inventory_format_sized((report), sizeof(*(report)), \
        (status), (json), (capacity), (size))
/* Validate and serialize before exclusive file creation. Require an absolute
 * ordinary path in a trusted existing directory. Never replace an existing
 * file or delete partial evidence after an I/O error. This returns persistence
 * status, separately from the discovery status stored inside the report.
 * Files contain local tool paths; choose who may receive this evidence. */
UmiStatus umi_toolchain_version_inventory_save_sized(
    const UmiToolchainDiscoveryReport *report, size_t report_size,
    UmiStatus discovery_status, const char *path);
#define umi_toolchain_version_inventory_save(report, status, path) \
    umi_toolchain_version_inventory_save_sized((report), sizeof(*(report)), (status), (path))
#ifdef __cplusplus
}
#endif
#endif
