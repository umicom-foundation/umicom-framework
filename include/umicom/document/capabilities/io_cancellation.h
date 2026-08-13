/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/io_cancellation.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Io Cancellation document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_IO_CANCELLATION_H
#define UMICOM_DOCUMENT_CAPABILITIES_IO_CANCELLATION_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_IO_CANCELLATION "umicom.document.performance.io_cancellation"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_io_cancellation(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_IO_CANCELLATION_H */
