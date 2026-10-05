/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/channel.c
 *
 * PURPOSE:
 *   release channel descriptors and stability ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/channel.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr channel from caller-provided values so later operations receive a known
 * state.
 */
void umi_dr_channel_init(UmiDrChannel *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrChannel){0}; value->kind=UMI_DR_CHANNEL_STABLE; value->stability_rank=4U; } }
/* Check that dr channel satisfies its contract before another service relies on it. */
bool umi_dr_channel_valid(const UmiDrChannel *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->kind != 0 && value->stability_rank>0U); }
/*
 * Provide the dr channel fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_channel_fingerprint(const UmiDrChannel *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_channel_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrChannelArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x186d8810d08a4b0a);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrChannel *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrChannelArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrChannel *)0)->id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDrChannelArchiveWrite(UmiArchiveWriter *writer, const UmiDrChannel *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->stability_rank);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->signed_only);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->automatic_updates);
}
static void UmiDrChannelArchiveRead(UmiArchiveReader *reader, UmiDrChannel *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->kind = (UmiDrChannelKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->stability_rank = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->signed_only = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->automatic_updates = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrChannelArchiveValidate(const UmiDrChannel *value)
{
    return umi_dr_channel_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_channel_archive_encode, umi_dr_channel_archive_decode,
    UmiDrChannel, UmiDrChannelArchiveSchema, UmiDrChannelArchiveBound, UmiDrChannelArchiveWrite, UmiDrChannelArchiveRead, UmiDrChannelArchiveValidate)
