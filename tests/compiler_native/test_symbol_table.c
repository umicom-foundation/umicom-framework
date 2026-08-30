/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_symbol_table.c
 *
 * PURPOSE:
 *   Regression coverage for provide scope-aware symbol registration and lexical name lookup with deterministic shadowing semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/symbol_table.h"
int main(void){UmiNativeSymbolTable t;umi_nc_symbol_table_init(&t);UmiNativeScope root,child;if(umi_nc_scope_init(&root,1U,0U,0U)!=UMI_STATUS_OK||umi_nc_scope_init(&child,2U,1U,1U)!=UMI_STATUS_OK)return 1;if(umi_nc_symbol_table_add_scope(&t,&root)!=UMI_STATUS_OK||umi_nc_symbol_table_add_scope(&t,&child)!=UMI_STATUS_OK)return 2;UmiNativeSymbol s;if(umi_nc_symbol_init(&s,1U,UMI_NC_SYMBOL_VARIABLE,"x",1U,1U)!=UMI_STATUS_OK)return 3;if(umi_nc_symbol_table_add_symbol(&t,&s)!=UMI_STATUS_OK)return 4;if(umi_nc_symbol_table_lookup(&t,2U,"x")==NULL)return 5;return 0;}
