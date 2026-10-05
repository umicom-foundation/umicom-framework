/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/provenance.c
 *
 * PURPOSE:
 *   build/source/toolchain provenance evidence for packaged releases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/provenance.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr provenance from caller-provided values so later operations receive a known
 * state.
 */
void umi_dr_provenance_init(UmiDrProvenance *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrProvenance){0};  } }
/* Check that dr provenance satisfies its contract before another service relies on it. */
bool umi_dr_provenance_valid(const UmiDrProvenance *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->source_revision, '\0', sizeof(value->source_revision)) == NULL) return 0;
    if (memchr(value->toolchain, '\0', sizeof(value->toolchain)) == NULL) return 0;
    if (memchr(value->builder, '\0', sizeof(value->builder)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->source_revision[0] != '\0' && value->toolchain[0] != '\0'); }
/*
 * Provide the dr provenance fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_provenance_fingerprint(const UmiDrProvenance *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_provenance_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrProvenanceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xbd8603ef27beb20e);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrProvenance *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrProvenance *)0)->source_revision)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrProvenance *)0)->toolchain)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrProvenance *)0)->builder)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrProvenanceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrProvenance *)0)->id) - 1U +
        8U + sizeof(((UmiDrProvenance *)0)->source_revision) - 1U +
        8U + sizeof(((UmiDrProvenance *)0)->toolchain) - 1U +
        8U + sizeof(((UmiDrProvenance *)0)->builder) - 1U +
        8U;
}
static void UmiDrProvenanceArchiveWrite(UmiArchiveWriter *writer, const UmiDrProvenance *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->source_revision, sizeof(value->source_revision));
    UmiArchiveWriteText(writer, value->toolchain, sizeof(value->toolchain));
    UmiArchiveWriteText(writer, value->builder, sizeof(value->builder));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->reproducible);
}
static void UmiDrProvenanceArchiveRead(UmiArchiveReader *reader, UmiDrProvenance *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->source_revision, sizeof(value->source_revision));
    UmiArchiveReadText(reader, value->toolchain, sizeof(value->toolchain));
    UmiArchiveReadText(reader, value->builder, sizeof(value->builder));
    value->reproducible = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrProvenanceArchiveValidate(const UmiDrProvenance *value)
{
    return umi_dr_provenance_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_provenance_archive_encode, umi_dr_provenance_archive_decode,
    UmiDrProvenance, UmiDrProvenanceArchiveSchema, UmiDrProvenanceArchiveBound, UmiDrProvenanceArchiveWrite, UmiDrProvenanceArchiveRead, UmiDrProvenanceArchiveValidate)
