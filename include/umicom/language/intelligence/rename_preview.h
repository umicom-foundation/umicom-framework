/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/rename_preview.h
 *
 * PURPOSE:
 *   Summarise and validate rename-preview edit counts before mutation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_RENAME_PREVIEW_H
#define UMICOM_LANGUAGE_INTELLIGENCE_RENAME_PREVIEW_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_RENAME_PREVIEW_API_VERSION 1U
typedef enum UmiLanguageIntelligenceRenamePreviewDecision {
    UMI_LANGUAGE_INTELLIGENCE_RENAME_PREVIEW_DENY = 0,
    UMI_LANGUAGE_INTELLIGENCE_RENAME_PREVIEW_ALLOW = 1,
    UMI_LANGUAGE_INTELLIGENCE_RENAME_PREVIEW_ALLOW_WITH_REFRESH = 2
} UmiLanguageIntelligenceRenamePreviewDecision;
typedef struct UmiLanguageIntelligenceRenamePreviewInput {
    UmiLanguageIntelligenceCapabilityFlags required_flags;
    UmiLanguageIntelligenceCapabilityFlags available_flags;
    uint32_t risk;
    uint32_t stale_age;
    uint32_t threshold;
    int hard_block;
    int explicit_override;
} UmiLanguageIntelligenceRenamePreviewInput;
UmiLanguageIntelligenceRenamePreviewDecision umi_language_intelligence_rename_preview_evaluate(
    const UmiLanguageIntelligenceRenamePreviewInput *input);
int umi_language_intelligence_rename_preview_capabilities_satisfied(
    const UmiLanguageIntelligenceRenamePreviewInput *input);
#ifdef __cplusplus
}
#endif
#endif
