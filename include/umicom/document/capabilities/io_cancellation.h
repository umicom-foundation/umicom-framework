/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/io_cancellation.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Io Cancellation document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_IO_CANCELLATION_H
#define UMICOM_DOCUMENT_CAPABILITIES_IO_CANCELLATION_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_IO_CANCELLATION "umicom.document.performance.io_cancellation"

/**
 * Provide the document capability io cancellation operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_io_cancellation(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_IO_CANCELLATION_H */
