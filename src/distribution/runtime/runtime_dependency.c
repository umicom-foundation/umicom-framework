/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/runtime_dependency.c
 *
 * PURPOSE:
 *   native runtime-library dependency and availability policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_dependency.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr runtime dependency from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_runtime_dependency_init(UmiDrRuntimeDependency *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrRuntimeDependency){0};  } }
/*
 * Check that dr runtime dependency satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_runtime_dependency_valid(const UmiDrRuntimeDependency *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->soname, '\0', sizeof(value->soname)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->soname[0] != '\0' && (value->system_provided || value->bundled)); }
/*
 * Provide the dr runtime dependency fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_runtime_dependency_fingerprint(const UmiDrRuntimeDependency *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_runtime_dependency_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrRuntimeDependencyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1ba0976fd5283bb2);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrRuntimeDependency *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrRuntimeDependency *)0)->soname)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrRuntimeDependencyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrRuntimeDependency *)0)->id) - 1U +
        8U + sizeof(((UmiDrRuntimeDependency *)0)->soname) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDrRuntimeDependencyArchiveWrite(UmiArchiveWriter *writer, const UmiDrRuntimeDependency *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->soname, sizeof(value->soname));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_version.major);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_version.minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_version.patch);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->system_provided);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->bundled);
}
static void UmiDrRuntimeDependencyArchiveRead(UmiArchiveReader *reader, UmiDrRuntimeDependency *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->soname, sizeof(value->soname));
    value->minimum_version.major = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->minimum_version.minor = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->minimum_version.patch = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->system_provided = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->bundled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrRuntimeDependencyArchiveValidate(const UmiDrRuntimeDependency *value)
{
    return umi_dr_runtime_dependency_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_runtime_dependency_archive_encode, umi_dr_runtime_dependency_archive_decode,
    UmiDrRuntimeDependency, UmiDrRuntimeDependencyArchiveSchema, UmiDrRuntimeDependencyArchiveBound, UmiDrRuntimeDependencyArchiveWrite, UmiDrRuntimeDependencyArchiveRead, UmiDrRuntimeDependencyArchiveValidate)
