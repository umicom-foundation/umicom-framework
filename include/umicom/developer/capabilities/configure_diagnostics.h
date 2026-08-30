/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/configure_diagnostics.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Configure Diagnostics developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_CONFIGURE_DIAGNOSTICS_H
#define UMICOM_DEVELOPER_CAPABILITIES_CONFIGURE_DIAGNOSTICS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_CONFIGURE_DIAGNOSTICS "umicom.developer.configuration.configure_diagnostics"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_configure_diagnostics(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_CONFIGURE_DIAGNOSTICS_H */
