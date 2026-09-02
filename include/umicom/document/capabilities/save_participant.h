/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/save_participant.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Save Participant document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_SAVE_PARTICIPANT_H
#define UMICOM_DOCUMENT_CAPABILITIES_SAVE_PARTICIPANT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_SAVE_PARTICIPANT "umicom.document.persistence.save_participant"

/**
 * Provide the document capability save participant operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_save_participant(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_SAVE_PARTICIPANT_H */
