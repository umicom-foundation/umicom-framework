/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/hot_exit.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Hot Exit document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_HOT_EXIT_H
#define UMICOM_DOCUMENT_CAPABILITIES_HOT_EXIT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_HOT_EXIT "umicom.document.recovery.hot_exit"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_hot_exit(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_HOT_EXIT_H */
