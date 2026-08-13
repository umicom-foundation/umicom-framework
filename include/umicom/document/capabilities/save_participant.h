/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/save_participant.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Save Participant document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_SAVE_PARTICIPANT_H
#define UMICOM_DOCUMENT_CAPABILITIES_SAVE_PARTICIPANT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_SAVE_PARTICIPANT "umicom.document.persistence.save_participant"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_save_participant(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_SAVE_PARTICIPANT_H */
