/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/maintenance/remote_status.c
 *
 * PURPOSE:
 *   Implement repository remote configuration validation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/remote_status.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise repository remote status from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_remote_status_init(UmiRepositoryRemoteStatus *status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != NULL) (void)memset(status, 0, sizeof(*status));
}

/*
 * Check that repository remote status satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_repository_remote_status_validate(const UmiRepositoryRemoteStatus *status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status->has_origin || status->upstream_configured || status->fetch_available) &&
        status->remote_count == 0U) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRepositoryRemoteStatusArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb4f60725580bf97a);

    return schema;
}
static size_t UmiRepositoryRemoteStatusArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRepositoryRemoteStatusArchiveWrite(UmiArchiveWriter *writer, const UmiRepositoryRemoteStatus *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->remote_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->has_origin);
    UmiArchiveWriteSigned(writer, (int64_t)value->upstream_configured);
    UmiArchiveWriteSigned(writer, (int64_t)value->fetch_available);
}
static void UmiRepositoryRemoteStatusArchiveRead(UmiArchiveReader *reader, UmiRepositoryRemoteStatus *value)
{
    value->remote_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->has_origin = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->upstream_configured = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->fetch_available = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiRepositoryRemoteStatusArchiveValidate(const UmiRepositoryRemoteStatus *value)
{
    return umi_repository_remote_status_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_repository_remote_status_archive_encode, umi_repository_remote_status_archive_decode,
    UmiRepositoryRemoteStatus, UmiRepositoryRemoteStatusArchiveSchema, UmiRepositoryRemoteStatusArchiveBound, UmiRepositoryRemoteStatusArchiveWrite, UmiRepositoryRemoteStatusArchiveRead, UmiRepositoryRemoteStatusArchiveValidate)
