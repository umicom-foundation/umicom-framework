/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/install_prefix.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Install Prefix developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_INSTALL_PREFIX_H
#define UMICOM_DEVELOPER_CAPABILITIES_INSTALL_PREFIX_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_INSTALL_PREFIX "umicom.developer.delivery.install_prefix"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_install_prefix(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_INSTALL_PREFIX_H */
