/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/configuration_layout.c
 *
 * PURPOSE:
 *   system, user and portable configuration-root policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/configuration_layout.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr configuration layout from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_configuration_layout_init(UmiDrConfigurationLayout *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrConfigurationLayout){0};  } }
/*
 * Check that dr configuration layout satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_configuration_layout_valid(const UmiDrConfigurationLayout *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->system_root, '\0', sizeof(value->system_root)) == NULL) return 0;
    if (memchr(value->user_root, '\0', sizeof(value->user_root)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->user_root[0] != '\0' && (value->portable || value->system_root[0] != '\0')); }
/*
 * Provide the dr configuration layout fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_configuration_layout_fingerprint(const UmiDrConfigurationLayout *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_configuration_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrConfigurationLayoutArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xcace7a40afeb6dac);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrConfigurationLayout *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrConfigurationLayout *)0)->system_root)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrConfigurationLayout *)0)->user_root)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrConfigurationLayoutArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrConfigurationLayout *)0)->id) - 1U +
        8U + sizeof(((UmiDrConfigurationLayout *)0)->system_root) - 1U +
        8U + sizeof(((UmiDrConfigurationLayout *)0)->user_root) - 1U +
        8U;
}
static void UmiDrConfigurationLayoutArchiveWrite(UmiArchiveWriter *writer, const UmiDrConfigurationLayout *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->system_root, sizeof(value->system_root));
    UmiArchiveWriteText(writer, value->user_root, sizeof(value->user_root));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->portable);
}
static void UmiDrConfigurationLayoutArchiveRead(UmiArchiveReader *reader, UmiDrConfigurationLayout *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->system_root, sizeof(value->system_root));
    UmiArchiveReadText(reader, value->user_root, sizeof(value->user_root));
    value->portable = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrConfigurationLayoutArchiveValidate(const UmiDrConfigurationLayout *value)
{
    return umi_dr_configuration_layout_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_configuration_layout_archive_encode, umi_dr_configuration_layout_archive_decode,
    UmiDrConfigurationLayout, UmiDrConfigurationLayoutArchiveSchema, UmiDrConfigurationLayoutArchiveBound, UmiDrConfigurationLayoutArchiveWrite, UmiDrConfigurationLayoutArchiveRead, UmiDrConfigurationLayoutArchiveValidate)
