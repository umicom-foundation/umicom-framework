/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/code_lens.h
 *
 * PURPOSE:
 *   Represent code-lens commands without toolkit or LSP transport ownership.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_CODE_LENS_H
#define UMICOM_LANGUAGE_INTELLIGENCE_CODE_LENS_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_CODE_LENS_API_VERSION 1U

typedef struct UmiLanguageIntelligenceCodeLens {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char subject_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char detail[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    UmiLanguageIntelligenceRange range;
    uint64_t revision;
    uint32_t priority;
    uint32_t flags;
    int enabled;
} UmiLanguageIntelligenceCodeLens;

void umi_language_intelligence_code_lens_init(
    UmiLanguageIntelligenceCodeLens *value,
    const char *id);
UmiStatus umi_language_intelligence_code_lens_validate(
    const UmiLanguageIntelligenceCodeLens *value);
UmiStatus umi_language_intelligence_code_lens_set_subject(
    UmiLanguageIntelligenceCodeLens *value,
    const char *subject_id);
UmiStatus umi_language_intelligence_code_lens_set_detail(
    UmiLanguageIntelligenceCodeLens *value,
    const char *detail);
void umi_language_intelligence_code_lens_touch(UmiLanguageIntelligenceCodeLens *value);
int umi_language_intelligence_code_lens_same_identity(
    const UmiLanguageIntelligenceCodeLens *left,
    const UmiLanguageIntelligenceCodeLens *right);

#ifdef __cplusplus
}
#endif
#endif
