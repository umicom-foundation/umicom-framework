/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/feature_option_matrix.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Feature Option Matrix developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_FEATURE_OPTION_MATRIX_H
#define UMICOM_DEVELOPER_CAPABILITIES_FEATURE_OPTION_MATRIX_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_FEATURE_OPTION_MATRIX "umicom.developer.configuration.feature_option_matrix"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_feature_option_matrix(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_FEATURE_OPTION_MATRIX_H */
