/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/configuration.h
 *
 * PURPOSE:
 *   Describe one reusable configure/build/test/run/install/package profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_CONFIGURATION_H
#define UMICOM_DEVELOPER_WORKBENCH_CONFIGURATION_H

#include "umicom/developer_workbench/types.h"
#include "umicom/toolchain/build.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer workbench configuration data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperWorkbenchConfiguration {
    uint32_t structure_size;
    uint32_t api_version;
    char configuration_id[UMI_DEVELOPER_WORKBENCH_ID_CAPACITY];
    char title[UMI_DEVELOPER_WORKBENCH_TITLE_CAPACITY];
    char source_root[UMI_DEVELOPER_WORKBENCH_PATH_CAPACITY];
    char build_directory[UMI_DEVELOPER_WORKBENCH_PATH_CAPACITY];
    char preset[UMI_DEVELOPER_WORKBENCH_ID_CAPACITY];
    char configuration[UMI_DEVELOPER_WORKBENCH_ID_CAPACITY];
    char executable[UMI_DEVELOPER_WORKBENCH_PATH_CAPACITY];
    char install_prefix[UMI_DEVELOPER_WORKBENCH_PATH_CAPACITY];
    char package_target[UMI_DEVELOPER_WORKBENCH_ID_CAPACITY];
    int jobs;
    uint32_t timeout_ms;
    int requires_workspace_trust;
    uint64_t revision;
} UmiDeveloperWorkbenchConfiguration;

/**
 * Initialise developer workbench configuration from caller-provided values so later
 * operations receive a known state.
 */
void umi_developer_workbench_configuration_init(
    UmiDeveloperWorkbenchConfiguration *configuration,
    const char *configuration_id,
    const char *title);

/**
 * Check that developer workbench configuration satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_developer_workbench_configuration_validate(
    const UmiDeveloperWorkbenchConfiguration *configuration,
    char *out_message,
    size_t message_capacity);

/**
 * Provide the developer workbench configuration to build request operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_configuration_to_build_request(
    const UmiDeveloperWorkbenchConfiguration *configuration,
    UmiBuildRequest *out_request);

#ifdef __cplusplus
}
#endif

#endif
