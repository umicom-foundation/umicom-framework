/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/dependency_bundle.c
 *
 * PURPOSE:
 *   resolved dependency set for one application release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/dependency_bundle.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr dependency bundle from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_dependency_bundle_init(UmiDrDependencyBundle *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrDependencyBundle){0};  } }
/*
 * Check that dr dependency bundle satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_dependency_bundle_valid(const UmiDrDependencyBundle *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->application_id, '\0', sizeof(value->application_id)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0'); }
/*
 * Provide the dr dependency bundle fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_dependency_bundle_fingerprint(const UmiDrDependencyBundle *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_dependency_bundle_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrDependencyBundleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x58150b09b574415c);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrDependencyBundle *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrDependencyBundle *)0)->application_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrDependencyBundleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrDependencyBundle *)0)->id) - 1U +
        8U + sizeof(((UmiDrDependencyBundle *)0)->application_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiDrDependencyBundleArchiveWrite(UmiArchiveWriter *writer, const UmiDrDependencyBundle *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->resolved_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->optional_missing);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->fingerprint);
}
static void UmiDrDependencyBundleArchiveRead(UmiArchiveReader *reader, UmiDrDependencyBundle *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    value->resolved_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->optional_missing = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiDrDependencyBundleArchiveValidate(const UmiDrDependencyBundle *value)
{
    return umi_dr_dependency_bundle_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_dependency_bundle_archive_encode, umi_dr_dependency_bundle_archive_decode,
    UmiDrDependencyBundle, UmiDrDependencyBundleArchiveSchema, UmiDrDependencyBundleArchiveBound, UmiDrDependencyBundleArchiveWrite, UmiDrDependencyBundleArchiveRead, UmiDrDependencyBundleArchiveValidate)
