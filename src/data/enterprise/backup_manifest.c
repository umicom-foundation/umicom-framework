/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/backup_manifest.c
 *
 * PURPOSE:
 *   Record completed backup evidence including content fingerprint and byte count.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/backup_manifest.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_backup_manifest_init(UmiDataBackupManifest *item, const char *backup_id, uint64_t created_at, uint64_t schema_fingerprint, uint64_t content_fingerprint, uint64_t bytes_written) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->backup_id,sizeof(item->backup_id),backup_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->created_at=created_at;item->schema_fingerprint=schema_fingerprint;item->content_fingerprint=content_fingerprint;item->bytes_written=bytes_written;item->complete=true;
    return umi_data_backup_manifest_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_backup_manifest_validate(const UmiDataBackupManifest *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->backup_id[0] != '\0' && item->schema_fingerprint != 0U && item->content_fingerprint != 0U && item->bytes_written > 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
