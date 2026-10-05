/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/operation/requirement.c
 *
 * PURPOSE:
 *   Implement validation and defaults for scoped tool requirements.
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
#include "umicom/toolchain/requirement.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise toolchain requirement from caller-provided values so later operations receive
 * a known state.
 */
void umi_toolchain_requirement_init(UmiToolchainRequirement *requirement,
                                    UmiToolKind kind,
                                    int required)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (requirement == NULL) return;
    requirement->kind = kind;
    requirement->required = required != 0;
    requirement->validate_version = 1;
}

/*
 * Check that toolchain requirement satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_toolchain_requirement_validate(const UmiToolchainRequirement *requirement)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (requirement == NULL || requirement->kind < 0 ||
        requirement->kind >= UMI_TOOL_COUNT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiToolchainRequirementArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x024f97dc4ad9a155);

    return schema;
}
static size_t UmiToolchainRequirementArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiToolchainRequirementArchiveWrite(UmiArchiveWriter *writer, const UmiToolchainRequirement *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->required);
    UmiArchiveWriteSigned(writer, (int64_t)value->validate_version);
}
static void UmiToolchainRequirementArchiveRead(UmiArchiveReader *reader, UmiToolchainRequirement *value)
{
    value->kind = (UmiToolKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->required = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->validate_version = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiToolchainRequirementArchiveValidate(const UmiToolchainRequirement *value)
{
    return umi_toolchain_requirement_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_toolchain_requirement_archive_encode, umi_toolchain_requirement_archive_decode,
    UmiToolchainRequirement, UmiToolchainRequirementArchiveSchema, UmiToolchainRequirementArchiveBound, UmiToolchainRequirementArchiveWrite, UmiToolchainRequirementArchiveRead, UmiToolchainRequirementArchiveValidate)
