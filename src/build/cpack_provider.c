/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/cpack_provider.c
 * PURPOSE: Build an owned argv for CPack without executing a command shell.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/cpack_provider.h"
#include "umicom/platform/filesystem.h"

static UmiStatus CreatePackageCommand(const UmiBuildProfile *profile,
    UmiBuildPhase phase, UmiBuildCommand *outCommand)
{
    if (profile == NULL || outCommand == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (phase != UMI_BUILD_PHASE_PACKAGE) return UMI_STATUS_NOT_IMPLEMENTED;
    UmiStatus status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    char config[UMI_BUILD_PATH_CAPACITY];
    char output[UMI_BUILD_PATH_CAPACITY];
    status = umi_fs_join(config, sizeof(config), profile->build_directory, "CPackConfig.cmake");
    if (status == UMI_STATUS_OK)
        status = umi_fs_join(output, sizeof(output), profile->build_directory, "packages");
    if (status != UMI_STATUS_OK) return status;
    UmiBuildCommand command;
    umi_build_command_init(&command, "cpack");
    const char *arguments[] = {"--config", config, "-G", "ZIP", "-C",
        profile->configuration, "-B", output, "-D", "CPACK_PACKAGE_CHECKSUM=SHA256"};
    for (size_t index = 0U; index < sizeof(arguments) / sizeof(arguments[0]); ++index)
        if (!umi_build_command_add_argument(&command, arguments[index]))
            return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!umi_build_command_set_working_directory(&command, profile->source_directory))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* argv entries point inside their owning record: rebind after the copy. */
    *outCommand = command;
    umi_build_command_rebind(outCommand);
    return UMI_STATUS_OK;
}

UmiBuildProvider UmiBuildCPackProvider(void)
{
    UmiBuildProvider provider = {0};
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.provider_id = "umicom.build.cpack";
    provider.supported_phases = UMI_BUILD_PHASE_MASK(UMI_BUILD_PHASE_PACKAGE);
    provider.create_command = CreatePackageCommand;
    return provider;
}
