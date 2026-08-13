/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/run_configurations.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Run Configurations developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_RUN_CONFIGURATIONS_H
#define UMICOM_DEVELOPER_CAPABILITIES_RUN_CONFIGURATIONS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_RUN_CONFIGURATIONS "umicom.developer.experience.run_configurations"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_run_configurations(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_RUN_CONFIGURATIONS_H */
