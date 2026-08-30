/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/run_readiness.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Run Readiness developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_RUN_READINESS_H
#define UMICOM_DEVELOPER_CAPABILITIES_RUN_READINESS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_RUN_READINESS "umicom.developer.execution.run_readiness"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_run_readiness(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_RUN_READINESS_H */
