/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/run_configurations.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Run Configurations developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_RUN_CONFIGURATIONS_H
#define UMICOM_DEVELOPER_CAPABILITIES_RUN_CONFIGURATIONS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_RUN_CONFIGURATIONS "umicom.developer.experience.run_configurations"

/**
 * Provide the developer capability run configurations operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_run_configurations(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_RUN_CONFIGURATIONS_H */
