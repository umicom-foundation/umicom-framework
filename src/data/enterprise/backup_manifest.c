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
#include "../../base/value_archive_internal.h"
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
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->backup_id, '\0', sizeof(item->backup_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->backup_id[0] != '\0' && item->schema_fingerprint != 0U && item->content_fingerprint != 0U && item->bytes_written > 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataBackupManifestArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb4f70b3e5aa0dc9b);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataBackupManifest *)0)->backup_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataBackupManifestArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataBackupManifest *)0)->backup_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDataBackupManifestArchiveWrite(UmiArchiveWriter *writer, const UmiDataBackupManifest *value)
{
    UmiArchiveWriteText(writer, value->backup_id, sizeof(value->backup_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->created_at);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->schema_fingerprint);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->content_fingerprint);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->bytes_written);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->complete);
}
static void UmiDataBackupManifestArchiveRead(UmiArchiveReader *reader, UmiDataBackupManifest *value)
{
    UmiArchiveReadText(reader, value->backup_id, sizeof(value->backup_id));
    value->created_at = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->schema_fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->content_fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->bytes_written = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->complete = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataBackupManifestArchiveValidate(const UmiDataBackupManifest *value)
{
    return umi_data_backup_manifest_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_backup_manifest_archive_encode, umi_data_backup_manifest_archive_decode,
    UmiDataBackupManifest, UmiDataBackupManifestArchiveSchema, UmiDataBackupManifestArchiveBound, UmiDataBackupManifestArchiveWrite, UmiDataBackupManifestArchiveRead, UmiDataBackupManifestArchiveValidate)
