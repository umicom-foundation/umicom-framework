/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/bundle_layout.c
 *
 * PURPOSE:
 *   portable application bundle directory layout validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/bundle_layout.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr bundle layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_bundle_layout_init(UmiDrBundleLayout *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrBundleLayout){0};  } }
/* Check that dr bundle layout satisfies its contract before another service relies on it. */
bool umi_dr_bundle_layout_valid(const UmiDrBundleLayout *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->bin_dir, '\0', sizeof(value->bin_dir)) == NULL) return 0;
    if (memchr(value->lib_dir, '\0', sizeof(value->lib_dir)) == NULL) return 0;
    if (memchr(value->share_dir, '\0', sizeof(value->share_dir)) == NULL) return 0;
    if (memchr(value->state_dir, '\0', sizeof(value->state_dir)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->bin_dir[0] != '\0' && value->lib_dir[0] != '\0' && value->share_dir[0] != '\0'); }
/*
 * Provide the dr bundle layout fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_bundle_layout_fingerprint(const UmiDrBundleLayout *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_bundle_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrBundleLayoutArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfb31b2fbded854ea);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrBundleLayout *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrBundleLayout *)0)->bin_dir)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrBundleLayout *)0)->lib_dir)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrBundleLayout *)0)->share_dir)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrBundleLayout *)0)->state_dir)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrBundleLayoutArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrBundleLayout *)0)->id) - 1U +
        8U + sizeof(((UmiDrBundleLayout *)0)->bin_dir) - 1U +
        8U + sizeof(((UmiDrBundleLayout *)0)->lib_dir) - 1U +
        8U + sizeof(((UmiDrBundleLayout *)0)->share_dir) - 1U +
        8U + sizeof(((UmiDrBundleLayout *)0)->state_dir) - 1U;
}
static void UmiDrBundleLayoutArchiveWrite(UmiArchiveWriter *writer, const UmiDrBundleLayout *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->bin_dir, sizeof(value->bin_dir));
    UmiArchiveWriteText(writer, value->lib_dir, sizeof(value->lib_dir));
    UmiArchiveWriteText(writer, value->share_dir, sizeof(value->share_dir));
    UmiArchiveWriteText(writer, value->state_dir, sizeof(value->state_dir));
}
static void UmiDrBundleLayoutArchiveRead(UmiArchiveReader *reader, UmiDrBundleLayout *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->bin_dir, sizeof(value->bin_dir));
    UmiArchiveReadText(reader, value->lib_dir, sizeof(value->lib_dir));
    UmiArchiveReadText(reader, value->share_dir, sizeof(value->share_dir));
    UmiArchiveReadText(reader, value->state_dir, sizeof(value->state_dir));
}
static UmiStatus UmiDrBundleLayoutArchiveValidate(const UmiDrBundleLayout *value)
{
    return umi_dr_bundle_layout_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_bundle_layout_archive_encode, umi_dr_bundle_layout_archive_decode,
    UmiDrBundleLayout, UmiDrBundleLayoutArchiveSchema, UmiDrBundleLayoutArchiveBound, UmiDrBundleLayoutArchiveWrite, UmiDrBundleLayoutArchiveRead, UmiDrBundleLayoutArchiveValidate)
