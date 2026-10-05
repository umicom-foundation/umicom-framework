/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/cache_layout.c
 *
 * PURPOSE:
 *   cache namespace and eviction-budget configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/cache_layout.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr cache layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_cache_layout_init(UmiDrCacheLayout *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrCacheLayout){0};  } }
/* Check that dr cache layout satisfies its contract before another service relies on it. */
bool umi_dr_cache_layout_valid(const UmiDrCacheLayout *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->namespace_id, '\0', sizeof(value->namespace_id)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->namespace_id[0] != '\0' && value->max_bytes > 0U); }
/*
 * Provide the dr cache layout fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_cache_layout_fingerprint(const UmiDrCacheLayout *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_cache_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrCacheLayoutArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x79293b3735a2a113);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrCacheLayout *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrCacheLayout *)0)->namespace_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrCacheLayoutArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrCacheLayout *)0)->id) - 1U +
        8U + sizeof(((UmiDrCacheLayout *)0)->namespace_id) - 1U +
        8U +
        8U;
}
static void UmiDrCacheLayoutArchiveWrite(UmiArchiveWriter *writer, const UmiDrCacheLayout *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->namespace_id, sizeof(value->namespace_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->max_bytes);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->disposable);
}
static void UmiDrCacheLayoutArchiveRead(UmiArchiveReader *reader, UmiDrCacheLayout *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->namespace_id, sizeof(value->namespace_id));
    value->max_bytes = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->disposable = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrCacheLayoutArchiveValidate(const UmiDrCacheLayout *value)
{
    return umi_dr_cache_layout_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_cache_layout_archive_encode, umi_dr_cache_layout_archive_decode,
    UmiDrCacheLayout, UmiDrCacheLayoutArchiveSchema, UmiDrCacheLayoutArchiveBound, UmiDrCacheLayoutArchiveWrite, UmiDrCacheLayoutArchiveRead, UmiDrCacheLayoutArchiveValidate)
