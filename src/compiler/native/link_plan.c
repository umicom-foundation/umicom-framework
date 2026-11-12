/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/link_plan.c
 *
 * PURPOSE:
 *   Build deterministic native link plans from objects, libraries, search roots and output kind.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/link_plan.h"
#include "../../base/value_construction_internal.h"
#include <string.h>
#include "../catalogue_validation.h"

bool umi_nc_link_plan_valid(const UmiNativeLinkPlan *plan)
{
    if (plan == NULL || plan->input_count > UMI_NC_MAX_LINK_INPUTS ||
        plan->library_count > UMI_NC_MAX_LINK_LIBRARIES ||
        plan->search_path_count > UMI_NC_MAX_LINK_SEARCH_PATHS ||
        plan->kind < UMI_NC_LINK_EXECUTABLE || plan->kind > UMI_NC_LINK_STATIC ||
        !UmiCompilerCatalogueText(plan->output,sizeof(plan->output),true)) return false;
    for (size_t index = 0U; index < plan->input_count; ++index)
        if (!UmiCompilerCatalogueText(plan->inputs[index],sizeof(plan->inputs[index]),true)) return false;
    for (size_t index = 0U; index < plan->library_count; ++index)
        if (!UmiCompilerCatalogueText(plan->libraries[index],sizeof(plan->libraries[index]),true)) return false;
    for (size_t index = 0U; index < plan->search_path_count; ++index)
        if (!UmiCompilerCatalogueText(plan->search_paths[index],sizeof(plan->search_paths[index]),true)) return false;
    return true;
}

/*
 * Initialise nc link plan from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_nc_link_plan_init(UmiNativeLinkPlan *p,const char *out,UmiNativeLinkOutputKind k){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||out==NULL||out[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof(*p));p->kind=k;return umi_nc_copy_text(p->output,sizeof(p->output),out);}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
/* Retain the existing staged publication wrapper while admitting only named link output kinds and reusing the bounded native text helper. The previous implementation is retained for review. */
#if 0
static UmiStatus umi_nc_link_plan_init_build_value(UmiNativeLinkPlan *p,const char *out,UmiNativeLinkOutputKind k){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||out==NULL||out[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof(*p));p->kind=k;return umi_nc_copy_text(p->output,sizeof(p->output),out);}
#endif
static UmiStatus umi_nc_link_plan_init_build_value(UmiNativeLinkPlan *plan,const char *output,UmiNativeLinkOutputKind kind)
{
    if (output == NULL || output[0] == '\0' ||
        kind < UMI_NC_LINK_EXECUTABLE || kind > UMI_NC_LINK_STATIC) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(plan,0,sizeof(*plan));
    plan->kind = kind;
    return umi_nc_copy_text(plan->output,sizeof(plan->output),output);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_nc_link_plan_init, UmiNativeLinkPlan, p,
    (UmiNativeLinkPlan *p, const char *out, UmiNativeLinkOutputKind k),
    umi_nc_link_plan_init_build_value(candidate, out, k))

/*
 * Provide the nc link plan add input operation used by this module and its client
 * applications.
 */
/* Admit the complete initialized link plan before adding a bounded value; rejected additions leave both active and unused storage unchanged. The previous implementation is retained for review. */
#if 0
UmiStatus umi_nc_link_plan_add_input(UmiNativeLinkPlan *p,const char *path){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||path==NULL||path[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->input_count>=UMI_NC_MAX_LINK_INPUTS)return UMI_STATUS_CAPACITY_EXCEEDED;UmiStatus st=umi_nc_copy_text(p->inputs[p->input_count],sizeof(p->inputs[0]),path);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)p->input_count++;return st;}
#endif
UmiStatus umi_nc_link_plan_add_input(UmiNativeLinkPlan *plan,const char *path)
{
    if (plan == NULL || path == NULL || path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_nc_link_plan_valid(plan)) return UMI_STATUS_INVALID_STATE;
    if (plan->input_count == UMI_NC_MAX_LINK_INPUTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiStatus status = umi_nc_copy_text(plan->inputs[plan->input_count],sizeof(plan->inputs[0]),path);
    if (status == UMI_STATUS_OK) plan->input_count += 1U;
    return status;
}
/*
 * Provide the nc link plan add library operation used by this module and its client
 * applications.
 */
/* Admit the complete initialized link plan before adding a bounded value; rejected additions leave both active and unused storage unchanged. The previous implementation is retained for review. */
#if 0
UmiStatus umi_nc_link_plan_add_library(UmiNativeLinkPlan *p,const char *lib){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||lib==NULL||lib[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->library_count>=UMI_NC_MAX_LINK_LIBRARIES)return UMI_STATUS_CAPACITY_EXCEEDED;UmiStatus st=umi_nc_copy_text(p->libraries[p->library_count],sizeof(p->libraries[0]),lib);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)p->library_count++;return st;}
#endif
UmiStatus umi_nc_link_plan_add_library(UmiNativeLinkPlan *plan,const char *library)
{
    if (plan == NULL || library == NULL || library[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_nc_link_plan_valid(plan)) return UMI_STATUS_INVALID_STATE;
    if (plan->library_count == UMI_NC_MAX_LINK_LIBRARIES) return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiStatus status = umi_nc_copy_text(plan->libraries[plan->library_count],sizeof(plan->libraries[0]),library);
    if (status == UMI_STATUS_OK) plan->library_count += 1U;
    return status;
}
/*
 * Provide the nc link plan fingerprint operation used by this module and its client
 * applications.
 */
/* Validate every active extent and string before hashing; include ordered search paths that the earlier fingerprint omitted. The previous implementation is retained for review. */
#if 0
uint64_t umi_nc_link_plan_fingerprint(const UmiNativeLinkPlan *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return 0U;uint64_t h=umi_nc_hash_text(p->output)^(uint64_t)p->kind;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<p->input_count;i++)h^=umi_nc_hash_text(p->inputs[i])+(h<<6U)+(h>>2U);/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<p->library_count;i++)h^=umi_nc_hash_text(p->libraries[i])+(h<<6U)+(h>>2U);return h;}
#endif
uint64_t umi_nc_link_plan_fingerprint(const UmiNativeLinkPlan *plan)
{
    if (!umi_nc_link_plan_valid(plan)) return 0U;
    uint64_t hash = umi_nc_hash_text(plan->output) ^ (uint64_t)plan->kind;
    for (size_t index = 0U; index < plan->input_count; ++index)
        hash ^= umi_nc_hash_text(plan->inputs[index]) + (hash << 6U) + (hash >> 2U);
    for (size_t index = 0U; index < plan->library_count; ++index)
        hash ^= umi_nc_hash_text(plan->libraries[index]) + (hash << 6U) + (hash >> 2U);
    /* Preserve established fingerprints when no search roots are present.
     * Include the ordered search-root section when it can affect resolution. */
    if (plan->search_path_count != 0U) {
        hash ^= UINT64_C(0x736561726368) + (uint64_t)plan->search_path_count + (hash << 6U) + (hash >> 2U);
        for (size_t index = 0U; index < plan->search_path_count; ++index)
            hash ^= umi_nc_hash_text(plan->search_paths[index]) + (hash << 6U) + (hash >> 2U);
    }
    return hash;
}

/* Search roots are owned link inputs too; use the same admission and copy contract. */
UmiStatus umi_nc_link_plan_add_search_path(UmiNativeLinkPlan *plan,const char *path)
{
    if (plan == NULL || path == NULL || path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_nc_link_plan_valid(plan)) return UMI_STATUS_INVALID_STATE;
    if (plan->search_path_count == UMI_NC_MAX_LINK_SEARCH_PATHS) return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiStatus status = umi_nc_copy_text(plan->search_paths[plan->search_path_count],sizeof(plan->search_paths[0]),path);
    if (status == UMI_STATUS_OK) plan->search_path_count += 1U;
    return status;
}
