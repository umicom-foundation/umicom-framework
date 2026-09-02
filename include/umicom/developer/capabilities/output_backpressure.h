/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/output_backpressure.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Output Backpressure developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_OUTPUT_BACKPRESSURE_H
#define UMICOM_DEVELOPER_CAPABILITIES_OUTPUT_BACKPRESSURE_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_OUTPUT_BACKPRESSURE "umicom.developer.performance.output_backpressure"

/**
 * Provide the developer capability output backpressure operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_output_backpressure(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_OUTPUT_BACKPRESSURE_H */
