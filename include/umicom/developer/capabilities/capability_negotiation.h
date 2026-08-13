/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/capability_negotiation.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Capability Negotiation developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_CAPABILITY_NEGOTIATION_H
#define UMICOM_DEVELOPER_CAPABILITIES_CAPABILITY_NEGOTIATION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_CAPABILITY_NEGOTIATION "umicom.developer.portability.capability_negotiation"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_capability_negotiation(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_CAPABILITY_NEGOTIATION_H */
