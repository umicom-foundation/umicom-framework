/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/latency_metrics.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Latency Metrics developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_LATENCY_METRICS_H
#define UMICOM_DEVELOPER_CAPABILITIES_LATENCY_METRICS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_LATENCY_METRICS "umicom.developer.performance.latency_metrics"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_latency_metrics(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_LATENCY_METRICS_H */
