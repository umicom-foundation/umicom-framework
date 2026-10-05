/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/bundle_file.c
 *
 * PURPOSE:
 *   individual bundle-file path, size, checksum and executable metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/bundle_file.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr bundle file from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_bundle_file_init(UmiDrBundleFile *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrBundleFile){0};  } }
/* Check that dr bundle file satisfies its contract before another service relies on it. */
bool umi_dr_bundle_file_valid(const UmiDrBundleFile *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->path, '\0', sizeof(value->path)) == NULL) return 0;
    if (memchr(value->digest, '\0', sizeof(value->digest)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->path[0] != '\0' && value->digest[0] != '\0' && value->size_bytes>0U); }
/*
 * Provide the dr bundle file fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_bundle_file_fingerprint(const UmiDrBundleFile *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_bundle_file_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrBundleFileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc2b515c8fe084753);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrBundleFile *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrBundleFile *)0)->path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrBundleFile *)0)->digest)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrBundleFileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrBundleFile *)0)->id) - 1U +
        8U + sizeof(((UmiDrBundleFile *)0)->path) - 1U +
        8U + sizeof(((UmiDrBundleFile *)0)->digest) - 1U +
        8U +
        8U;
}
static void UmiDrBundleFileArchiveWrite(UmiArchiveWriter *writer, const UmiDrBundleFile *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->path, sizeof(value->path));
    UmiArchiveWriteText(writer, value->digest, sizeof(value->digest));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->size_bytes);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->executable);
}
static void UmiDrBundleFileArchiveRead(UmiArchiveReader *reader, UmiDrBundleFile *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->path, sizeof(value->path));
    UmiArchiveReadText(reader, value->digest, sizeof(value->digest));
    value->size_bytes = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->executable = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrBundleFileArchiveValidate(const UmiDrBundleFile *value)
{
    return umi_dr_bundle_file_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_bundle_file_archive_encode, umi_dr_bundle_file_archive_decode,
    UmiDrBundleFile, UmiDrBundleFileArchiveSchema, UmiDrBundleFileArchiveBound, UmiDrBundleFileArchiveWrite, UmiDrBundleFileArchiveRead, UmiDrBundleFileArchiveValidate)
