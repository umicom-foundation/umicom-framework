/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/installation_inspection.c
 * PURPOSE: Inspect a relocatable SDK using its own declared layout and base library.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/sdk/installation_inspection.h"
#include "umicom/platform/filesystem.h"
#include <stdlib.h>
#include <string.h>

/* Join through the shared path service so all output capacities are checked.
 * Presence checks follow trusted installation aliases but never evaluate code. */
static UmiStatus InstallationFile(const char *directory, const char *name, int *present)
{
    char path[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_join(directory, name, path, sizeof path);
    if (status == UMI_STATUS_OK) *present = umi_fs_is_file(path);
    return status;
}

UmiStatus UmiSdkInstallationInspect(const char *prefix, UmiSdkInstallationObservation *out_observation)
{
    if (out_observation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_observation, 0, sizeof *out_observation);
    if (prefix == NULL || !umi_path_is_absolute(prefix)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiSdkInstallationObservation *candidate = calloc(1U, sizeof *candidate);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_path_normalise(prefix, candidate->prefix, sizeof candidate->prefix);
    if (status == UMI_STATUS_OK)
        status = UmiSdkInstalledMetadataLoad(candidate->prefix, &candidate->metadata);
    char directory[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK)
        status = umi_path_join(candidate->prefix, candidate->metadata.library_directory,
            directory, sizeof directory);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(directory, "cmake/UmicomFramework",
            candidate->package_directory, sizeof candidate->package_directory);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(candidate->prefix, candidate->metadata.include_directory,
            directory, sizeof directory);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(directory, "umicom/umicom.h", candidate->public_header,
            sizeof candidate->public_header);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(candidate->prefix, candidate->metadata.base_library,
            candidate->base_library, sizeof candidate->base_library);
    if (status == UMI_STATUS_OK)
        status = InstallationFile(candidate->package_directory, "UmicomFrameworkConfig.cmake",
            &candidate->package_config);
    if (status == UMI_STATUS_OK)
        status = InstallationFile(candidate->package_directory, "UmicomFrameworkTargets.cmake",
            &candidate->package_targets);
    if (status == UMI_STATUS_OK)
        status = InstallationFile(candidate->package_directory, "UmicomFrameworkConfigVersion.cmake",
            &candidate->package_version);
    if (status == UMI_STATUS_OK) {
        candidate->header_present = umi_fs_is_file(candidate->public_header);
        candidate->base_library_present = umi_fs_is_file(candidate->base_library);
        /* An empty library folder cannot satisfy the base-library requirement.
         * The compiler and linker still have to validate its bytes and target ABI. */
        candidate->complete = candidate->package_config && candidate->package_targets &&
            candidate->package_version && candidate->header_present && candidate->base_library_present;
        *out_observation = *candidate;
    }
    free(candidate);
    return status;
}
