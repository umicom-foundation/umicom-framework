/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/compatibility.c
 *
 * PURPOSE:
 *   Compare required and available ABI descriptors and count incompatibilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source file implements one small part of ABI validation. Keeping checks separate makes release failures easier to understand and test.
 */
#include "umicom/abi/compatibility.h"
#include <string.h>
static int has_feature(const UmiAbiDescriptor *d,const UmiAbiFeature *f){size_t i;for(i=0U;i<d->feature_count;++i)if(strcmp(d->features[i].feature_id,f->feature_id)==0&&d->features[i].version>=f->version)return 1;return 0;}
static int has_symbol(const UmiAbiDescriptor *d,const UmiAbiSymbol *s){size_t i;for(i=0U;i<d->symbol_count;++i)if(strcmp(d->symbols[i].name,s->name)==0)return 1;return 0;}
UmiStatus umi_abi_compatibility_check(const UmiAbiDescriptor *r,const UmiAbiDescriptor *a,UmiAbiCompatibility *o){size_t i,j;if(o==NULL||umi_abi_descriptor_validate(r)!=UMI_STATUS_OK||umi_abi_descriptor_validate(a)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;o->missing_features=0U;o->missing_symbols=0U;o->incompatible_layouts=0U;if(r->abi_version>a->abi_version||!umi_abi_platform_compatible(&r->platform,&a->platform)){o->compatible=0;return UMI_STATUS_OK;}for(i=0U;i<r->feature_count;++i)if(r->features[i].required&&!has_feature(a,&r->features[i]))++o->missing_features;for(i=0U;i<r->symbol_count;++i)if(r->symbols[i].required&&!has_symbol(a,&r->symbols[i]))++o->missing_symbols;for(i=0U;i<r->layout_count;++i){int found=0;for(j=0U;j<a->layout_count;++j)if(umi_abi_struct_layout_compatible(&r->layouts[i],&a->layouts[j])){found=1;break;}if(!found)++o->incompatible_layouts;}o->compatible=(o->missing_features==0U&&o->missing_symbols==0U&&o->incompatible_layouts==0U);return UMI_STATUS_OK;}
