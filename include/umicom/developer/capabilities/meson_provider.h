/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/meson_provider.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Meson Provider developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_MESON_PROVIDER_H
#define UMICOM_DEVELOPER_CAPABILITIES_MESON_PROVIDER_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_MESON_PROVIDER "umicom.developer.integration.meson_provider"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_meson_provider(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_MESON_PROVIDER_H */
