/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/runtime_profile.c
 *
 * PURPOSE:
 *   named runtime profiles combining platform, architecture and capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_profile.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr runtime profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_runtime_profile_init(UmiDrRuntimeProfile *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrRuntimeProfile){0}; value->platform=UMI_DR_PLATFORM_LINUX; value->architecture=UMI_DR_ARCH_X86_64; } }
/*
 * Check that dr runtime profile satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_runtime_profile_valid(const UmiDrRuntimeProfile *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->platform != 0 && value->architecture != 0); }
/*
 * Provide the dr runtime profile fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_runtime_profile_fingerprint(const UmiDrRuntimeProfile *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_runtime_profile_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrRuntimeProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3991004fa3795394);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrRuntimeProfile *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrRuntimeProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrRuntimeProfile *)0)->id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDrRuntimeProfileArchiveWrite(UmiArchiveWriter *writer, const UmiDrRuntimeProfile *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->platform);
    UmiArchiveWriteSigned(writer, (int64_t)value->architecture);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_version.major);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_version.minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_version.patch);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required_capabilities);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->score_bias);
}
static void UmiDrRuntimeProfileArchiveRead(UmiArchiveReader *reader, UmiDrRuntimeProfile *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->platform = (UmiDrPlatform)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->architecture = (UmiDrArchitecture)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->minimum_version.major = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->minimum_version.minor = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->minimum_version.patch = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->required_capabilities = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->score_bias = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiDrRuntimeProfileArchiveValidate(const UmiDrRuntimeProfile *value)
{
    return umi_dr_runtime_profile_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_runtime_profile_archive_encode, umi_dr_runtime_profile_archive_decode,
    UmiDrRuntimeProfile, UmiDrRuntimeProfileArchiveSchema, UmiDrRuntimeProfileArchiveBound, UmiDrRuntimeProfileArchiveWrite, UmiDrRuntimeProfileArchiveRead, UmiDrRuntimeProfileArchiveValidate)
