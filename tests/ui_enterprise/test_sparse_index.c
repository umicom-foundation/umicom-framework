/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_sparse_index.c
 *
 * PURPOSE:
 *   Exercise the sparse index enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/sparse_index.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntSparseIndex x;size_t n;umi_ui_ent_sparse_index_init(&x);umi_ui_ent_sparse_index_put(&x,8U,80U);umi_ui_ent_sparse_index_put(&x,2U,20U);/* Apply this branch only when its contract condition is satisfied. */ if(!umi_ui_ent_sparse_index_find(&x,8U,&n)||n!=80U)return 1;puts("ok");return 0;}
