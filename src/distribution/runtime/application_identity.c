/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/application_identity.c
 *
 * PURPOSE:
 *   stable application identity, publisher and product-family metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/application_identity.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr application identity from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_application_identity_init(UmiDrApplicationIdentity *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrApplicationIdentity){0};  } }
/*
 * Check that dr application identity satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_application_identity_valid(const UmiDrApplicationIdentity *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->publisher, '\0', sizeof(value->publisher)) == NULL) return 0;
    if (memchr(value->family, '\0', sizeof(value->family)) == NULL) return 0;
    if (memchr(value->product, '\0', sizeof(value->product)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->publisher[0] != '\0' && value->product[0] != '\0'); }
/*
 * Provide the dr application identity fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_application_identity_fingerprint(const UmiDrApplicationIdentity *value) {
    uint64_t h = 0U;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_dr_application_identity_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrApplicationIdentityArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1885b59e88eb2ee4);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrApplicationIdentity *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrApplicationIdentity *)0)->publisher)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrApplicationIdentity *)0)->family)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrApplicationIdentity *)0)->product)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrApplicationIdentityArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrApplicationIdentity *)0)->id) - 1U +
        8U + sizeof(((UmiDrApplicationIdentity *)0)->publisher) - 1U +
        8U + sizeof(((UmiDrApplicationIdentity *)0)->family) - 1U +
        8U + sizeof(((UmiDrApplicationIdentity *)0)->product) - 1U;
}
static void UmiDrApplicationIdentityArchiveWrite(UmiArchiveWriter *writer, const UmiDrApplicationIdentity *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->publisher, sizeof(value->publisher));
    UmiArchiveWriteText(writer, value->family, sizeof(value->family));
    UmiArchiveWriteText(writer, value->product, sizeof(value->product));
}
static void UmiDrApplicationIdentityArchiveRead(UmiArchiveReader *reader, UmiDrApplicationIdentity *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->publisher, sizeof(value->publisher));
    UmiArchiveReadText(reader, value->family, sizeof(value->family));
    UmiArchiveReadText(reader, value->product, sizeof(value->product));
}
static UmiStatus UmiDrApplicationIdentityArchiveValidate(const UmiDrApplicationIdentity *value)
{
    return umi_dr_application_identity_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_application_identity_archive_encode, umi_dr_application_identity_archive_decode,
    UmiDrApplicationIdentity, UmiDrApplicationIdentityArchiveSchema, UmiDrApplicationIdentityArchiveBound, UmiDrApplicationIdentityArchiveWrite, UmiDrApplicationIdentityArchiveRead, UmiDrApplicationIdentityArchiveValidate)
