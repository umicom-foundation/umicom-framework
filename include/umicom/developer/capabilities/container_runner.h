/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/container_runner.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Container Runner developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_CONTAINER_RUNNER_H
#define UMICOM_DEVELOPER_CAPABILITIES_CONTAINER_RUNNER_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_CONTAINER_RUNNER "umicom.developer.integration.container_runner"

/**
 * Provide the developer capability container runner operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_container_runner(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_CONTAINER_RUNNER_H */
