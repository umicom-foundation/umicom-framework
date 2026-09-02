/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/argument_quoting.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Argument Quoting developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_ARGUMENT_QUOTING_H
#define UMICOM_DEVELOPER_CAPABILITIES_ARGUMENT_QUOTING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_ARGUMENT_QUOTING "umicom.developer.execution.argument_quoting"

/**
 * Provide the developer capability argument quoting operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_argument_quoting(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_ARGUMENT_QUOTING_H */
