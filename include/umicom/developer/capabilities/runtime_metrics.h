/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/runtime_metrics.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Runtime Metrics developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_RUNTIME_METRICS_H
#define UMICOM_DEVELOPER_CAPABILITIES_RUNTIME_METRICS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_RUNTIME_METRICS "umicom.developer.diagnostics.runtime_metrics"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_runtime_metrics(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_RUNTIME_METRICS_H */
