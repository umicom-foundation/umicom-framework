/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/plugin_trust.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Plugin Trust developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PLUGIN_TRUST_H
#define UMICOM_DEVELOPER_CAPABILITIES_PLUGIN_TRUST_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PLUGIN_TRUST "umicom.developer.security.plugin_trust"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_plugin_trust(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PLUGIN_TRUST_H */
