/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/document_telemetry.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Document Telemetry document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_TELEMETRY_H
#define UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_TELEMETRY_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_DOCUMENT_TELEMETRY "umicom.document.integration.document_telemetry"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_document_telemetry(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_TELEMETRY_H */
