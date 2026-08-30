/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/api_baseline.c
 *
 * PURPOSE:
 *   Implement the api baseline behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Public API baseline comparison | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/codeguard/api_baseline.h"
#include <string.h>
UmiStatus umi_codeguard_api_baseline_init(UmiCodeGuardApiBaseline *baseline,const char *name,unsigned major,unsigned minor)
{
    if (baseline == NULL || name == NULL || name[0] == '\0' || major == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(baseline,0,sizeof(*baseline));
    baseline->major = major; baseline->minor = minor;
    return umi_codeguard_quality_copy(baseline->name,sizeof(baseline->name),name);
}
const UmiCodeGuardApiSymbol *umi_codeguard_api_baseline_find(const UmiCodeGuardApiBaseline *baseline,const char *symbol)
{
    size_t index;
    if (baseline == NULL || symbol == NULL) return NULL;
    for (index = 0U; index < baseline->count; ++index) if (strcmp(baseline->symbols[index].name,symbol) == 0) return &baseline->symbols[index];
    return NULL;
}
UmiStatus umi_codeguard_api_baseline_add(UmiCodeGuardApiBaseline *baseline,const char *symbol,uint64_t signature_hash,bool deprecated)
{
    UmiStatus status;
    if (baseline == NULL || symbol == NULL || symbol[0] == '\0' || signature_hash == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_codeguard_api_baseline_find(baseline,symbol) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (baseline->count >= UMI_CODEGUARD_QUALITY_MAX_SYMBOLS) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_codeguard_quality_copy(baseline->symbols[baseline->count].name,sizeof(baseline->symbols[baseline->count].name),symbol);
    if (status == UMI_STATUS_OK) { baseline->symbols[baseline->count].signature_hash = signature_hash; baseline->symbols[baseline->count].deprecated = deprecated; baseline->count += 1U; }
    return status;
}
static UmiStatus add_change(UmiCodeGuardApiComparison *comparison,const char *symbol,UmiCodeGuardCompatibilityLevel level,const char *summary)
{
    UmiCodeGuardApiChange *change;
    if (comparison->count >= UMI_CODEGUARD_QUALITY_MAX_CHANGES) return UMI_STATUS_CAPACITY_EXCEEDED;
    change = &comparison->changes[comparison->count];
    (void)umi_codeguard_quality_copy(change->symbol,sizeof(change->symbol),symbol);
    change->level = level;
    (void)umi_codeguard_quality_copy(change->summary,sizeof(change->summary),summary);
    comparison->count += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_codeguard_api_baseline_compare(const UmiCodeGuardApiBaseline *expected,const UmiCodeGuardApiBaseline *current,UmiCodeGuardApiComparison *out_comparison)
{
    size_t index;
    if (expected == NULL || current == NULL || out_comparison == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_comparison,0,sizeof(*out_comparison));
    for (index = 0U; index < expected->count; ++index) {
        const UmiCodeGuardApiSymbol *now = umi_codeguard_api_baseline_find(current,expected->symbols[index].name);
        if (now == NULL) { UmiCodeGuardCompatibilityLevel level = expected->symbols[index].deprecated ? UMI_CODEGUARD_COMPATIBILITY_WARNING : UMI_CODEGUARD_BREAKING; if (add_change(out_comparison,expected->symbols[index].name,level,"Public symbol removed from the current baseline") != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; out_comparison->removals += 1U; }
        else if (now->signature_hash != expected->symbols[index].signature_hash) { if (add_change(out_comparison,expected->symbols[index].name,UMI_CODEGUARD_BREAKING,"Public symbol signature changed") != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; out_comparison->signature_changes += 1U; }
    }
    for (index = 0U; index < current->count; ++index) if (umi_codeguard_api_baseline_find(expected,current->symbols[index].name) == NULL) { if (add_change(out_comparison,current->symbols[index].name,UMI_CODEGUARD_COMPATIBLE,"Public symbol added") != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; out_comparison->additions += 1U; }
    return UMI_STATUS_OK;
}
bool umi_codeguard_api_comparison_breaking(const UmiCodeGuardApiComparison *comparison)
{
    size_t index;
    if (comparison == NULL) return true;
    for (index = 0U; index < comparison->count; ++index) if (comparison->changes[index].level == UMI_CODEGUARD_BREAKING) return true;
    return false;
}
