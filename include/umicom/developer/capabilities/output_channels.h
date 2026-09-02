/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/output_channels.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Output Channels developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_OUTPUT_CHANNELS_H
#define UMICOM_DEVELOPER_CAPABILITIES_OUTPUT_CHANNELS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_OUTPUT_CHANNELS "umicom.developer.diagnostics.output_channels"

/**
 * Provide the developer capability output channels operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_output_channels(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_OUTPUT_CHANNELS_H */
