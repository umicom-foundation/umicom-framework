/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/environment_probe.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Environment Probe developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_ENVIRONMENT_PROBE_H
#define UMICOM_DEVELOPER_CAPABILITIES_ENVIRONMENT_PROBE_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_ENVIRONMENT_PROBE "umicom.developer.configuration.environment_probe"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_environment_probe(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_ENVIRONMENT_PROBE_H */
