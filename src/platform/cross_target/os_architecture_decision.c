/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/os_architecture_decision.c
 *
 * PURPOSE:
 *   Provide and validate the accepted Umicom OS architecture decision.
 *
 * ARCHITECTURE:
 *   This source models ownership and release policy. It does not boot a
 *   kernel, call privileged instructions or make Framework a kernel library.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/cross_target/os_architecture_decision.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Fill every field explicitly so a debugger or UI can explain the accepted
 * production, portability and research tracks from one authoritative record. */
UmiStatus umi_ct_umicom_os_architecture_decision_default(
    UmiCtOsArchitectureDecision *out_decision)
{
    /* A caller must provide storage because this function never allocates or
     * returns a pointer whose lifetime would need separate management. */
    if (out_decision == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_decision, 0, sizeof(*out_decision));
    out_decision->structure_size = (uint32_t)sizeof(*out_decision);
    out_decision->api_version = UMI_CT_API_VERSION;
    out_decision->production_foundation =
        UMI_CT_OS_FOUNDATION_LINUX_LTS;
    out_decision->portability_foundation =
        UMI_CT_OS_FOUNDATION_FREEBSD;
    out_decision->research_foundation =
        UMI_CT_OS_FOUNDATION_UMICOM_MICROKERNEL;
    out_decision->kernel_uses_framework = false;
    out_decision->recovery_uses_framework = false;
    out_decision->normal_user_space_uses_framework = true;
    out_decision->freestanding_subset_allowed = true;
    out_decision->separate_kernel_repository = true;
    out_decision->separate_distribution_repository = true;
    out_decision->research_is_product_default = false;
    return UMI_STATUS_OK;
}

/* The accepted values are deliberately strict. Changing one is an
 * architecture decision and therefore requires code, tests and documentation
 * to change together rather than drifting through an unchecked flag. */
UmiStatus umi_ct_umicom_os_architecture_decision_validate(
    const UmiCtOsArchitectureDecision *decision)
{
    /* Size and API checks stop code compiled for a different contract shape
     * from reading a field that is absent or has a different meaning. */
    if (decision == NULL ||
        decision->structure_size != (uint32_t)sizeof(*decision) ||
        decision->api_version != UMI_CT_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Foundation choices are part of the accepted architecture. A deliberate
     * change must update this validator, its tests and the decision record. */
    if (decision->production_foundation !=
            UMI_CT_OS_FOUNDATION_LINUX_LTS ||
        decision->portability_foundation !=
            UMI_CT_OS_FOUNDATION_FREEBSD ||
        decision->research_foundation !=
            UMI_CT_OS_FOUNDATION_UMICOM_MICROKERNEL) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* These dependency and repository rules keep boot and recovery independent
     * while preventing experimental work from silently becoming production. */
    if (decision->kernel_uses_framework ||
        decision->recovery_uses_framework ||
        !decision->normal_user_space_uses_framework ||
        !decision->freestanding_subset_allowed ||
        !decision->separate_kernel_repository ||
        !decision->separate_distribution_repository ||
        decision->research_is_product_default) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* Translate the enum only at presentation boundaries; internal code keeps the
 * enum so comparisons do not depend on spelling or localisation. */
const char *umi_ct_os_foundation_text(UmiCtOsFoundation foundation)
{
    /* Each known enum has stable human-readable text for logs and controls. */
    switch (foundation) {
    case UMI_CT_OS_FOUNDATION_LINUX_LTS:
        return "Linux LTS";
    case UMI_CT_OS_FOUNDATION_FREEBSD:
        return "FreeBSD";
    case UMI_CT_OS_FOUNDATION_UMICOM_MICROKERNEL:
        return "Umicom microkernel";
    default:
        /* Unknown values remain printable, which makes corrupt or newer data
         * diagnosable without dereferencing missing storage. */
        return "unknown";
    }
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtOsArchitectureDecisionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x04b25193d6bc745a);

    return schema;
}
static size_t UmiCtOsArchitectureDecisionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCtOsArchitectureDecisionArchiveWrite(UmiArchiveWriter *writer, const UmiCtOsArchitectureDecision *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteSigned(writer, (int64_t)value->production_foundation);
    UmiArchiveWriteSigned(writer, (int64_t)value->portability_foundation);
    UmiArchiveWriteSigned(writer, (int64_t)value->research_foundation);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->kernel_uses_framework);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->recovery_uses_framework);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->normal_user_space_uses_framework);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->freestanding_subset_allowed);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->separate_kernel_repository);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->separate_distribution_repository);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->research_is_product_default);
}
static void UmiCtOsArchitectureDecisionArchiveRead(UmiArchiveReader *reader, UmiCtOsArchitectureDecision *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->production_foundation = (UmiCtOsFoundation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->portability_foundation = (UmiCtOsFoundation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->research_foundation = (UmiCtOsFoundation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->kernel_uses_framework = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->recovery_uses_framework = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->normal_user_space_uses_framework = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->freestanding_subset_allowed = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->separate_kernel_repository = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->separate_distribution_repository = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->research_is_product_default = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCtOsArchitectureDecisionArchiveValidate(const UmiCtOsArchitectureDecision *value)
{
    return umi_ct_umicom_os_architecture_decision_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_umicom_os_architecture_decision_archive_encode, umi_ct_umicom_os_architecture_decision_archive_decode,
    UmiCtOsArchitectureDecision, UmiCtOsArchitectureDecisionArchiveSchema, UmiCtOsArchitectureDecisionArchiveBound, UmiCtOsArchitectureDecisionArchiveWrite, UmiCtOsArchitectureDecisionArchiveRead, UmiCtOsArchitectureDecisionArchiveValidate)
