/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/secret_redaction.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Secret Redaction developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_SECRET_REDACTION_H
#define UMICOM_DEVELOPER_CAPABILITIES_SECRET_REDACTION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_SECRET_REDACTION "umicom.developer.security.secret_redaction"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_secret_redaction(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_SECRET_REDACTION_H */
