/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/data_layout.c
 *
 * PURPOSE:
 *   read-only packaged data and writable application-data separation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/data_layout.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr data layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_data_layout_init(UmiDrDataLayout *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrDataLayout){0};  } }
/* Check that dr data layout satisfies its contract before another service relies on it. */
bool umi_dr_data_layout_valid(const UmiDrDataLayout *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->read_only_dir, '\0', sizeof(value->read_only_dir)) == NULL) return 0;
    if (memchr(value->writable_dir, '\0', sizeof(value->writable_dir)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->read_only_dir[0] != '\0' && value->writable_dir[0] != '\0'); }
/*
 * Provide the dr data layout fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_data_layout_fingerprint(const UmiDrDataLayout *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_data_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrDataLayoutArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe92043f9ec4ec04f);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrDataLayout *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrDataLayout *)0)->read_only_dir)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrDataLayout *)0)->writable_dir)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrDataLayoutArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrDataLayout *)0)->id) - 1U +
        8U + sizeof(((UmiDrDataLayout *)0)->read_only_dir) - 1U +
        8U + sizeof(((UmiDrDataLayout *)0)->writable_dir) - 1U +
        8U;
}
static void UmiDrDataLayoutArchiveWrite(UmiArchiveWriter *writer, const UmiDrDataLayout *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->read_only_dir, sizeof(value->read_only_dir));
    UmiArchiveWriteText(writer, value->writable_dir, sizeof(value->writable_dir));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->migrate_legacy);
}
static void UmiDrDataLayoutArchiveRead(UmiArchiveReader *reader, UmiDrDataLayout *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->read_only_dir, sizeof(value->read_only_dir));
    UmiArchiveReadText(reader, value->writable_dir, sizeof(value->writable_dir));
    value->migrate_legacy = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrDataLayoutArchiveValidate(const UmiDrDataLayout *value)
{
    return umi_dr_data_layout_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_data_layout_archive_encode, umi_dr_data_layout_archive_decode,
    UmiDrDataLayout, UmiDrDataLayoutArchiveSchema, UmiDrDataLayoutArchiveBound, UmiDrDataLayoutArchiveWrite, UmiDrDataLayoutArchiveRead, UmiDrDataLayoutArchiveValidate)
