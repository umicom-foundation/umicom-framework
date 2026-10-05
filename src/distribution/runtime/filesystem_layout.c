/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/filesystem_layout.c
 *
 * PURPOSE:
 *   canonical install-root, bin, lib, share and writable-state layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/filesystem_layout.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr filesystem layout from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_filesystem_layout_init(UmiDrFilesystemLayout *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrFilesystemLayout){0};  } }
/*
 * Check that dr filesystem layout satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_filesystem_layout_valid(const UmiDrFilesystemLayout *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->root, '\0', sizeof(value->root)) == NULL) return 0;
    if (memchr(value->bin, '\0', sizeof(value->bin)) == NULL) return 0;
    if (memchr(value->lib, '\0', sizeof(value->lib)) == NULL) return 0;
    if (memchr(value->share, '\0', sizeof(value->share)) == NULL) return 0;
    if (memchr(value->state, '\0', sizeof(value->state)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->root[0] != '\0' && value->bin[0] != '\0' && value->lib[0] != '\0'); }
/*
 * Provide the dr filesystem layout fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_filesystem_layout_fingerprint(const UmiDrFilesystemLayout *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_filesystem_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrFilesystemLayoutArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0ee525c5e9ba2eff);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrFilesystemLayout *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrFilesystemLayout *)0)->root)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrFilesystemLayout *)0)->bin)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrFilesystemLayout *)0)->lib)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrFilesystemLayout *)0)->share)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrFilesystemLayout *)0)->state)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrFilesystemLayoutArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrFilesystemLayout *)0)->id) - 1U +
        8U + sizeof(((UmiDrFilesystemLayout *)0)->root) - 1U +
        8U + sizeof(((UmiDrFilesystemLayout *)0)->bin) - 1U +
        8U + sizeof(((UmiDrFilesystemLayout *)0)->lib) - 1U +
        8U + sizeof(((UmiDrFilesystemLayout *)0)->share) - 1U +
        8U + sizeof(((UmiDrFilesystemLayout *)0)->state) - 1U;
}
static void UmiDrFilesystemLayoutArchiveWrite(UmiArchiveWriter *writer, const UmiDrFilesystemLayout *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->root, sizeof(value->root));
    UmiArchiveWriteText(writer, value->bin, sizeof(value->bin));
    UmiArchiveWriteText(writer, value->lib, sizeof(value->lib));
    UmiArchiveWriteText(writer, value->share, sizeof(value->share));
    UmiArchiveWriteText(writer, value->state, sizeof(value->state));
}
static void UmiDrFilesystemLayoutArchiveRead(UmiArchiveReader *reader, UmiDrFilesystemLayout *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->root, sizeof(value->root));
    UmiArchiveReadText(reader, value->bin, sizeof(value->bin));
    UmiArchiveReadText(reader, value->lib, sizeof(value->lib));
    UmiArchiveReadText(reader, value->share, sizeof(value->share));
    UmiArchiveReadText(reader, value->state, sizeof(value->state));
}
static UmiStatus UmiDrFilesystemLayoutArchiveValidate(const UmiDrFilesystemLayout *value)
{
    return umi_dr_filesystem_layout_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_filesystem_layout_archive_encode, umi_dr_filesystem_layout_archive_decode,
    UmiDrFilesystemLayout, UmiDrFilesystemLayoutArchiveSchema, UmiDrFilesystemLayoutArchiveBound, UmiDrFilesystemLayoutArchiveWrite, UmiDrFilesystemLayoutArchiveRead, UmiDrFilesystemLayoutArchiveValidate)
