/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/run_history.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Run History developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_RUN_HISTORY_H
#define UMICOM_DEVELOPER_CAPABILITIES_RUN_HISTORY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_RUN_HISTORY "umicom.developer.execution.run_history"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_run_history(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_RUN_HISTORY_H */
