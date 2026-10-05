/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/launch_profile.c
 *
 * PURPOSE:
 *   named launch profile with environment, frontend and safe-mode controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/launch_profile.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr launch profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_launch_profile_init(UmiDrLaunchProfile *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrLaunchProfile){0};  } }
/* Check that dr launch profile satisfies its contract before another service relies on it. */
bool umi_dr_launch_profile_valid(const UmiDrLaunchProfile *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->launcher_id, '\0', sizeof(value->launcher_id)) == NULL) return 0;
    if (memchr(value->environment_id, '\0', sizeof(value->environment_id)) == NULL) return 0;
    if (memchr(value->frontend, '\0', sizeof(value->frontend)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->launcher_id[0] != '\0'); }
/*
 * Provide the dr launch profile fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_launch_profile_fingerprint(const UmiDrLaunchProfile *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_launch_profile_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrLaunchProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x90b233e69877cc90);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrLaunchProfile *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrLaunchProfile *)0)->launcher_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrLaunchProfile *)0)->environment_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrLaunchProfile *)0)->frontend)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrLaunchProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrLaunchProfile *)0)->id) - 1U +
        8U + sizeof(((UmiDrLaunchProfile *)0)->launcher_id) - 1U +
        8U + sizeof(((UmiDrLaunchProfile *)0)->environment_id) - 1U +
        8U + sizeof(((UmiDrLaunchProfile *)0)->frontend) - 1U +
        8U +
        8U;
}
static void UmiDrLaunchProfileArchiveWrite(UmiArchiveWriter *writer, const UmiDrLaunchProfile *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->launcher_id, sizeof(value->launcher_id));
    UmiArchiveWriteText(writer, value->environment_id, sizeof(value->environment_id));
    UmiArchiveWriteText(writer, value->frontend, sizeof(value->frontend));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->safe_mode);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->offline);
}
static void UmiDrLaunchProfileArchiveRead(UmiArchiveReader *reader, UmiDrLaunchProfile *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->launcher_id, sizeof(value->launcher_id));
    UmiArchiveReadText(reader, value->environment_id, sizeof(value->environment_id));
    UmiArchiveReadText(reader, value->frontend, sizeof(value->frontend));
    value->safe_mode = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->offline = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrLaunchProfileArchiveValidate(const UmiDrLaunchProfile *value)
{
    return umi_dr_launch_profile_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_launch_profile_archive_encode, umi_dr_launch_profile_archive_decode,
    UmiDrLaunchProfile, UmiDrLaunchProfileArchiveSchema, UmiDrLaunchProfileArchiveBound, UmiDrLaunchProfileArchiveWrite, UmiDrLaunchProfileArchiveRead, UmiDrLaunchProfileArchiveValidate)
