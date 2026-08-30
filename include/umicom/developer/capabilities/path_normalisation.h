/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/path_normalisation.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Path Normalisation developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PATH_NORMALISATION_H
#define UMICOM_DEVELOPER_CAPABILITIES_PATH_NORMALISATION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PATH_NORMALISATION "umicom.developer.portability.path_normalisation"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_path_normalisation(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PATH_NORMALISATION_H */
