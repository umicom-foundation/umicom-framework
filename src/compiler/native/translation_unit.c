/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/translation_unit.c
 *
 * PURPOSE:
 *   Track top-level declarations and source identity for a single C translation unit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/translation_unit.h"
#include "../../base/value_construction_internal.h"
#include <string.h>
/*
 * Initialise nc translation unit from caller-provided values so later operations receive a
 * known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_nc_translation_unit_init(UmiNativeTranslationUnit *u,const char *path,uint32_t root,uint64_t fp){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(u==NULL||path==NULL||root==0U||fp==0U) return UMI_STATUS_INVALID_ARGUMENT; memset(u,0,sizeof(*u)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(u->path,sizeof(u->path),path)!=UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;u->root_node_id=root;u->source_fingerprint=fp;return UMI_STATUS_OK; }
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_nc_translation_unit_init_build_value(UmiNativeTranslationUnit *u,const char *path,uint32_t root,uint64_t fp){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(u==NULL||path==NULL||root==0U||fp==0U) return UMI_STATUS_INVALID_ARGUMENT; memset(u,0,sizeof(*u)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(u->path,sizeof(u->path),path)!=UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;u->root_node_id=root;u->source_fingerprint=fp;return UMI_STATUS_OK; }

UMI_DEFINE_STAGED_VALUE_INIT(umi_nc_translation_unit_init, UmiNativeTranslationUnit, u,
    (UmiNativeTranslationUnit *u, const char *path, uint32_t root, uint64_t fp),
    umi_nc_translation_unit_init_build_value(candidate, path, root, fp))

/*
 * Provide the nc translation unit add declaration operation used by this module and its
 * client applications.
 */
UmiStatus umi_nc_translation_unit_add_declaration(UmiNativeTranslationUnit *u,uint32_t id){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(u==NULL||id==0U)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(u->declaration_count>=UMI_NC_MAX_TOP_LEVEL_DECLARATIONS)return UMI_STATUS_CAPACITY_EXCEEDED;u->declaration_ids[u->declaration_count++]=id;return UMI_STATUS_OK; }
/*
 * Provide the nc translation unit fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_nc_translation_unit_fingerprint(const UmiNativeTranslationUnit *u){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(u==NULL)return 0U;uint64_t h=u->source_fingerprint^umi_nc_hash_text(u->path);/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<u->declaration_count;i++){h^=(uint64_t)u->declaration_ids[i]+(h<<6U)+(h>>2U);}return h; }
