/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/distributed_tracing.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Distributed Tracing developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_DISTRIBUTED_TRACING_H
#define UMICOM_DEVELOPER_CAPABILITIES_DISTRIBUTED_TRACING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_DISTRIBUTED_TRACING "umicom.developer.diagnostics.distributed_tracing"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_distributed_tracing(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_DISTRIBUTED_TRACING_H */
