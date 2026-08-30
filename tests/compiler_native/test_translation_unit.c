/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_translation_unit.c
 *
 * PURPOSE:
 *   Regression coverage for track top-level declarations and source identity for a single c translation unit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/translation_unit.h"
int main(void){ UmiNativeTranslationUnit u; if(umi_nc_translation_unit_init(&u,"main.c",1U,9U)!=UMI_STATUS_OK)return 1; if(umi_nc_translation_unit_add_declaration(&u,2U)!=UMI_STATUS_OK)return 2; if(umi_nc_translation_unit_fingerprint(&u)==0U)return 3;return 0; }
