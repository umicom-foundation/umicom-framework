/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/install_receipt.c
 *
 * PURPOSE:
 *   immutable installation receipt and package fingerprint evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/install_receipt.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr install receipt from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_install_receipt_init(UmiDrInstallReceipt *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrInstallReceipt){0};  } }
/*
 * Check that dr install receipt satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_install_receipt_valid(const UmiDrInstallReceipt *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->application_id, '\0', sizeof(value->application_id)) == NULL) return 0;
    if (memchr(value->package_digest, '\0', sizeof(value->package_digest)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && value->package_digest[0] != '\0' && value->installed_at>0U); }
/*
 * Provide the dr install receipt fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_install_receipt_fingerprint(const UmiDrInstallReceipt *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_install_receipt_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrInstallReceiptArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe7591d4933c7d0ca);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrInstallReceipt *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrInstallReceipt *)0)->application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrInstallReceipt *)0)->package_digest)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrInstallReceiptArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrInstallReceipt *)0)->id) - 1U +
        8U + sizeof(((UmiDrInstallReceipt *)0)->application_id) - 1U +
        8U + sizeof(((UmiDrInstallReceipt *)0)->package_digest) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDrInstallReceiptArchiveWrite(UmiArchiveWriter *writer, const UmiDrInstallReceipt *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->package_digest, sizeof(value->package_digest));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->version.major);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->version.minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->version.patch);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->installed_at);
}
static void UmiDrInstallReceiptArchiveRead(UmiArchiveReader *reader, UmiDrInstallReceipt *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->package_digest, sizeof(value->package_digest));
    value->version.major = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->version.minor = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->version.patch = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->installed_at = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiDrInstallReceiptArchiveValidate(const UmiDrInstallReceipt *value)
{
    return umi_dr_install_receipt_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_install_receipt_archive_encode, umi_dr_install_receipt_archive_decode,
    UmiDrInstallReceipt, UmiDrInstallReceiptArchiveSchema, UmiDrInstallReceiptArchiveBound, UmiDrInstallReceiptArchiveWrite, UmiDrInstallReceiptArchiveRead, UmiDrInstallReceiptArchiveValidate)
