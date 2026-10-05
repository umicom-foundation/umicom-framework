/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/library_layout.c
 *
 * PURPOSE:
 *   shared/private runtime library placement policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/library_layout.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr library layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_library_layout_init(UmiDrLibraryLayout *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrLibraryLayout){0};  } }
/* Check that dr library layout satisfies its contract before another service relies on it. */
bool umi_dr_library_layout_valid(const UmiDrLibraryLayout *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->private_dir, '\0', sizeof(value->private_dir)) == NULL) return 0;
    if (memchr(value->system_hint, '\0', sizeof(value->system_hint)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->private_dir[0] != '\0' && value->search_relative); }
/*
 * Provide the dr library layout fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_library_layout_fingerprint(const UmiDrLibraryLayout *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_library_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrLibraryLayoutArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xca3e810c1aec4b3c);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrLibraryLayout *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrLibraryLayout *)0)->private_dir)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrLibraryLayout *)0)->system_hint)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrLibraryLayoutArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrLibraryLayout *)0)->id) - 1U +
        8U + sizeof(((UmiDrLibraryLayout *)0)->private_dir) - 1U +
        8U + sizeof(((UmiDrLibraryLayout *)0)->system_hint) - 1U +
        8U;
}
static void UmiDrLibraryLayoutArchiveWrite(UmiArchiveWriter *writer, const UmiDrLibraryLayout *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->private_dir, sizeof(value->private_dir));
    UmiArchiveWriteText(writer, value->system_hint, sizeof(value->system_hint));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->search_relative);
}
static void UmiDrLibraryLayoutArchiveRead(UmiArchiveReader *reader, UmiDrLibraryLayout *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->private_dir, sizeof(value->private_dir));
    UmiArchiveReadText(reader, value->system_hint, sizeof(value->system_hint));
    value->search_relative = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrLibraryLayoutArchiveValidate(const UmiDrLibraryLayout *value)
{
    return umi_dr_library_layout_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_library_layout_archive_encode, umi_dr_library_layout_archive_decode,
    UmiDrLibraryLayout, UmiDrLibraryLayoutArchiveSchema, UmiDrLibraryLayoutArchiveBound, UmiDrLibraryLayoutArchiveWrite, UmiDrLibraryLayoutArchiveRead, UmiDrLibraryLayoutArchiveValidate)
