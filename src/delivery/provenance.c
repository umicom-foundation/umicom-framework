/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/provenance.c
 *
 * PURPOSE:
 *   Record source revision, builder identity and build inputs for release provenance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Provenance links a package back to the exact source and build environment that created it.
 */

#include "umicom/delivery/provenance.h"
#include "../base/value_archive_internal.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise provenance from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_provenance_init(UmiProvenance *provenance,
                              const char *source_revision,
                              const char *builder_id,
                              const char *build_preset)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provenance == NULL || source_revision == NULL || builder_id == NULL ||
        build_preset == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(provenance, 0, sizeof(*provenance));
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_delivery_copy_text(provenance->source_revision,
                               sizeof(provenance->source_revision),
                               source_revision) != UMI_STATUS_OK ||
        umi_delivery_copy_text(provenance->builder_id,
                               sizeof(provenance->builder_id),
                               builder_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(provenance->build_preset,
                               sizeof(provenance->build_preset),
                               build_preset) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/* Check that provenance satisfies its contract before another service relies on it. */
UmiStatus umi_provenance_validate(const UmiProvenance *provenance)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (provenance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(provenance->source_revision, '\0', sizeof(provenance->source_revision)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(provenance->builder_id, '\0', sizeof(provenance->builder_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(provenance->build_preset, '\0', sizeof(provenance->build_preset)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(provenance->framework_version, '\0', sizeof(provenance->framework_version)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provenance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return provenance->source_revision[0] != '\0' &&
           provenance->builder_id[0] != '\0'
               ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiProvenanceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7f4f161931a27ddb);
    schema = (schema ^ (uint64_t)sizeof(((UmiProvenance *)0)->source_revision)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProvenance *)0)->builder_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProvenance *)0)->build_preset)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProvenance *)0)->framework_version)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiProvenanceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiProvenance *)0)->source_revision) - 1U +
        8U + sizeof(((UmiProvenance *)0)->builder_id) - 1U +
        8U + sizeof(((UmiProvenance *)0)->build_preset) - 1U +
        8U + sizeof(((UmiProvenance *)0)->framework_version) - 1U +
        8U;
}
static void UmiProvenanceArchiveWrite(UmiArchiveWriter *writer, const UmiProvenance *value)
{
    UmiArchiveWriteText(writer, value->source_revision, sizeof(value->source_revision));
    UmiArchiveWriteText(writer, value->builder_id, sizeof(value->builder_id));
    UmiArchiveWriteText(writer, value->build_preset, sizeof(value->build_preset));
    UmiArchiveWriteText(writer, value->framework_version, sizeof(value->framework_version));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->created_epoch_ms);
}
static void UmiProvenanceArchiveRead(UmiArchiveReader *reader, UmiProvenance *value)
{
    UmiArchiveReadText(reader, value->source_revision, sizeof(value->source_revision));
    UmiArchiveReadText(reader, value->builder_id, sizeof(value->builder_id));
    UmiArchiveReadText(reader, value->build_preset, sizeof(value->build_preset));
    UmiArchiveReadText(reader, value->framework_version, sizeof(value->framework_version));
    value->created_epoch_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiProvenanceArchiveValidate(const UmiProvenance *value)
{
    return umi_provenance_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_provenance_archive_encode, umi_provenance_archive_decode,
    UmiProvenance, UmiProvenanceArchiveSchema, UmiProvenanceArchiveBound, UmiProvenanceArchiveWrite, UmiProvenanceArchiveRead, UmiProvenanceArchiveValidate)
