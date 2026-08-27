/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/acceptance/toolchain_gate.h
 *
 * PURPOSE:
 *   Translate existing language/toolchain support-matrix rows into explicit developer acceptance evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_ACCEPTANCE_TOOLCHAIN_GATE_H
#define UMICOM_DEVELOPER_ACCEPTANCE_TOOLCHAIN_GATE_H

#include "umicom/developer/acceptance/evidence.h"
#include "umicom/developer/language_support_matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Record one language support row under the stable capability id
 * umicom.developer.language.<language-id>. A row passes only when a binding
 * exists and every supported operation has an available tool.
 */
UmiStatus umi_developer_acceptance_record_toolchain_support(
    const UmiDeveloperLanguageSupportSnapshot *support,
    UmiDeveloperAcceptanceEvidenceStore *evidence);

#ifdef __cplusplus
}
#endif
#endif
