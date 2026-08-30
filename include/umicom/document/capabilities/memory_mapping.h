/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/memory_mapping.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Memory Mapping document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_MEMORY_MAPPING_H
#define UMICOM_DOCUMENT_CAPABILITIES_MEMORY_MAPPING_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_MEMORY_MAPPING "umicom.document.performance.memory_mapping"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_memory_mapping(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_MEMORY_MAPPING_H */
