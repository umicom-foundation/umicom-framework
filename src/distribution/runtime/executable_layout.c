/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/executable_layout.c
 *
 * PURPOSE:
 *   executable placement and launch-entry validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/executable_layout.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr executable layout from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_executable_layout_init(UmiDrExecutableLayout *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrExecutableLayout){0};  } }
/*
 * Check that dr executable layout satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_executable_layout_valid(const UmiDrExecutableLayout *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->entrypoint, '\0', sizeof(value->entrypoint)) == NULL) return 0;
    if (memchr(value->bin_dir, '\0', sizeof(value->bin_dir)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->entrypoint[0] != '\0' && value->bin_dir[0] != '\0'); }
/*
 * Provide the dr executable layout fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_executable_layout_fingerprint(const UmiDrExecutableLayout *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_executable_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrExecutableLayoutArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3a84f43517a19ff8);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrExecutableLayout *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrExecutableLayout *)0)->entrypoint)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrExecutableLayout *)0)->bin_dir)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrExecutableLayoutArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrExecutableLayout *)0)->id) - 1U +
        8U + sizeof(((UmiDrExecutableLayout *)0)->entrypoint) - 1U +
        8U + sizeof(((UmiDrExecutableLayout *)0)->bin_dir) - 1U +
        8U +
        8U;
}
static void UmiDrExecutableLayoutArchiveWrite(UmiArchiveWriter *writer, const UmiDrExecutableLayout *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->entrypoint, sizeof(value->entrypoint));
    UmiArchiveWriteText(writer, value->bin_dir, sizeof(value->bin_dir));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->console);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->gui);
}
static void UmiDrExecutableLayoutArchiveRead(UmiArchiveReader *reader, UmiDrExecutableLayout *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->entrypoint, sizeof(value->entrypoint));
    UmiArchiveReadText(reader, value->bin_dir, sizeof(value->bin_dir));
    value->console = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->gui = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrExecutableLayoutArchiveValidate(const UmiDrExecutableLayout *value)
{
    return umi_dr_executable_layout_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_executable_layout_archive_encode, umi_dr_executable_layout_archive_decode,
    UmiDrExecutableLayout, UmiDrExecutableLayoutArchiveSchema, UmiDrExecutableLayoutArchiveBound, UmiDrExecutableLayoutArchiveWrite, UmiDrExecutableLayoutArchiveRead, UmiDrExecutableLayoutArchiveValidate)
