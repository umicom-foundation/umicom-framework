/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/acceptance_rule.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/acceptance_rule.h"
#include "../../base/value_archive_internal.h"

/*
 * Provide the application production acceptance rule default operation used by this module
 * and its client applications.
 */
UmiApplicationProductionAcceptanceRule
umi_application_production_acceptance_rule_default(void)
{
    UmiApplicationProductionAcceptanceRule rule = {
        1, 1, 1, 1, 1, 1
    };
    return rule;
}

/*
 * Check that application production acceptance rule satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_application_production_acceptance_rule_validate(
    const UmiApplicationProductionAcceptanceRule *rule)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (rule == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!rule->require_manifest && !rule->require_layout_projection &&
        !rule->require_capabilities && !rule->require_tests &&
        !rule->require_evidence)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}


/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiApplicationProductionAcceptanceRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x93b489e088e59727);

    return schema;
}
static size_t UmiApplicationProductionAcceptanceRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiApplicationProductionAcceptanceRuleArchiveWrite(UmiArchiveWriter *writer, const UmiApplicationProductionAcceptanceRule *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->require_manifest);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_layout_projection);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_capabilities);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_tests);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_evidence);
    UmiArchiveWriteSigned(writer, (int64_t)value->allow_degraded_optional_capabilities);
}
static void UmiApplicationProductionAcceptanceRuleArchiveRead(UmiArchiveReader *reader, UmiApplicationProductionAcceptanceRule *value)
{
    value->require_manifest = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_layout_projection = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_capabilities = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_tests = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_evidence = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->allow_degraded_optional_capabilities = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiApplicationProductionAcceptanceRuleArchiveValidate(const UmiApplicationProductionAcceptanceRule *value)
{
    return umi_application_production_acceptance_rule_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_application_production_acceptance_rule_archive_encode, umi_application_production_acceptance_rule_archive_decode,
    UmiApplicationProductionAcceptanceRule, UmiApplicationProductionAcceptanceRuleArchiveSchema, UmiApplicationProductionAcceptanceRuleArchiveBound, UmiApplicationProductionAcceptanceRuleArchiveWrite, UmiApplicationProductionAcceptanceRuleArchiveRead, UmiApplicationProductionAcceptanceRuleArchiveValidate)
