/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/api_baseline.h
 *
 * PURPOSE:
 *   Publish the public api baseline contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_API_BASELINE_H
#define UMICOM_CODEGUARD_API_BASELINE_H
#include "umicom/codeguard/quality_types.h"
typedef struct UmiCodeGuardApiSymbol {
    char name[UMI_CODEGUARD_QUALITY_ID_CAPACITY];
    uint64_t signature_hash;
    bool deprecated;
} UmiCodeGuardApiSymbol;
typedef struct UmiCodeGuardApiBaseline {
    char name[UMI_CODEGUARD_QUALITY_ID_CAPACITY];
    unsigned major;
    unsigned minor;
    UmiCodeGuardApiSymbol symbols[UMI_CODEGUARD_QUALITY_MAX_SYMBOLS];
    size_t count;
} UmiCodeGuardApiBaseline;
typedef struct UmiCodeGuardApiChange {
    char symbol[UMI_CODEGUARD_QUALITY_ID_CAPACITY];
    UmiCodeGuardCompatibilityLevel level;
    char summary[UMI_CODEGUARD_QUALITY_TEXT_CAPACITY];
} UmiCodeGuardApiChange;
typedef struct UmiCodeGuardApiComparison {
    UmiCodeGuardApiChange changes[UMI_CODEGUARD_QUALITY_MAX_CHANGES];
    size_t count;
    size_t additions;
    size_t removals;
    size_t signature_changes;
} UmiCodeGuardApiComparison;
UmiStatus umi_codeguard_api_baseline_init(UmiCodeGuardApiBaseline *baseline,const char *name,unsigned major,unsigned minor);
UmiStatus umi_codeguard_api_baseline_add(UmiCodeGuardApiBaseline *baseline,const char *symbol,uint64_t signature_hash,bool deprecated);
const UmiCodeGuardApiSymbol *umi_codeguard_api_baseline_find(const UmiCodeGuardApiBaseline *baseline,const char *symbol);
UmiStatus umi_codeguard_api_baseline_compare(const UmiCodeGuardApiBaseline *expected,const UmiCodeGuardApiBaseline *current,UmiCodeGuardApiComparison *out_comparison);
bool umi_codeguard_api_comparison_breaking(const UmiCodeGuardApiComparison *comparison);
#endif
