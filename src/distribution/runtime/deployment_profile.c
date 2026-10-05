/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/deployment_profile.c
 *
 * PURPOSE:
 *   deployment target, scope, rollout and update-channel profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/deployment_profile.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr deployment profile from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_deployment_profile_init(UmiDrDeploymentProfile *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrDeploymentProfile){0};  } }
/*
 * Check that dr deployment profile satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_deployment_profile_valid(const UmiDrDeploymentProfile *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->target, '\0', sizeof(value->target)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->target[0] != '\0' && value->scope != 0 && value->channel != 0 && value->rollout_percent<=100U); }
/*
 * Provide the dr deployment profile fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_deployment_profile_fingerprint(const UmiDrDeploymentProfile *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_deployment_profile_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrDeploymentProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x819107e394a495e2);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrDeploymentProfile *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrDeploymentProfile *)0)->target)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrDeploymentProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrDeploymentProfile *)0)->id) - 1U +
        8U + sizeof(((UmiDrDeploymentProfile *)0)->target) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDrDeploymentProfileArchiveWrite(UmiArchiveWriter *writer, const UmiDrDeploymentProfile *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->target, sizeof(value->target));
    UmiArchiveWriteSigned(writer, (int64_t)value->scope);
    UmiArchiveWriteSigned(writer, (int64_t)value->channel);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->rollout_percent);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->unattended);
}
static void UmiDrDeploymentProfileArchiveRead(UmiArchiveReader *reader, UmiDrDeploymentProfile *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->target, sizeof(value->target));
    value->scope = (UmiDrInstallScope)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->channel = (UmiDrChannelKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->rollout_percent = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->unattended = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrDeploymentProfileArchiveValidate(const UmiDrDeploymentProfile *value)
{
    return umi_dr_deployment_profile_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_deployment_profile_archive_encode, umi_dr_deployment_profile_archive_decode,
    UmiDrDeploymentProfile, UmiDrDeploymentProfileArchiveSchema, UmiDrDeploymentProfileArchiveBound, UmiDrDeploymentProfileArchiveWrite, UmiDrDeploymentProfileArchiveRead, UmiDrDeploymentProfileArchiveValidate)
