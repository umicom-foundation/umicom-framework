/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/acceptance/toolchain_gate.c
 *
 * PURPOSE:
 *   Record existing language/toolchain readiness as acceptance evidence without changing capability or binding metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/acceptance/toolchain_gate.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_developer_acceptance_record_toolchain_support(
    const UmiDeveloperLanguageSupportSnapshot *support,
    UmiDeveloperAcceptanceEvidenceStore *evidence)
{
    UmiDeveloperAcceptanceEvidence item;

    if (support == NULL ||
        evidence == NULL ||
        support->language_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&item, 0, sizeof(item));

    (void)snprintf(
        item.capability_id,
        sizeof(item.capability_id),
        "umicom.developer.language.%s",
        support->language_id);
    (void)snprintf(
        item.check_id,
        sizeof(item.check_id),
        "toolchain-readiness.%s",
        support->language_id);

    item.state =
        support->binding_found &&
        support->supported_operation_count > 0U &&
        support->missing_operation_count == 0U
            ? UMI_DEVELOPER_ACCEPTANCE_PASS
            : UMI_DEVELOPER_ACCEPTANCE_BLOCKED;

    (void)snprintf(
        item.detail,
        sizeof(item.detail),
        "binding=%s supported=%zu ready=%zu missing=%zu readiness=%u%%",
        support->binding_id,
        support->supported_operation_count,
        support->ready_operation_count,
        support->missing_operation_count,
        support->readiness_percent);

    return umi_developer_acceptance_evidence_upsert(
        evidence,
        &item);
}
