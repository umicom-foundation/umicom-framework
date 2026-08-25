/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_text.h
 *
 * PURPOSE:
 *   Format remediation proposals as an explicit plan rather than hidden automation.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_TEXT_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_TEXT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/repository/remediation_types.h"
#include "umicom/repository/remediation_policy.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "umicom/repository/remediation_plan.h"
/* Format a plan into caller-owned text including risk labels and command previews. */
UmiStatus umi_repository_remediation_text_format(const UmiRepositoryRemediationPlan *plan,char *out_text,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
