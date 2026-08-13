/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/backup_copy.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Backup Copy document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_BACKUP_COPY_H
#define UMICOM_DOCUMENT_CAPABILITIES_BACKUP_COPY_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_BACKUP_COPY "umicom.document.persistence.backup_copy"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_backup_copy(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_BACKUP_COPY_H */
