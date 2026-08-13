/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/gtk4_document_host.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Gtk4 Document Host document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_GTK4_DOCUMENT_HOST_H
#define UMICOM_DOCUMENT_CAPABILITIES_GTK4_DOCUMENT_HOST_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_GTK4_DOCUMENT_HOST "umicom.document.integration.gtk4_document_host"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_gtk4_document_host(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_GTK4_DOCUMENT_HOST_H */
