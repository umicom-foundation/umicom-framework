/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/backup_rotation.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Backup Rotation document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_BACKUP_ROTATION_H
#define UMICOM_DOCUMENT_CAPABILITIES_BACKUP_ROTATION_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_BACKUP_ROTATION "umicom.document.recovery.backup_rotation"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_backup_rotation(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_BACKUP_ROTATION_H */
