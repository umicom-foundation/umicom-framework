/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_data_provider.c
 *
 * PURPOSE:
 *   Exercise the data provider enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/data_provider.h"
#include <stdio.h>
/*
 * Exercise rows and return a clear result when the behaviour no longer matches its
 * contract.
 */
static size_t rows(void *c){(void)c;return 2U;} static UmiStatus cell(void *c,size_t r,size_t col,UmiUiEntCellValue *out){(void)c;(void)col;umi_ui_ent_cell_value_set_integer(out,(int64_t)r);return UMI_STATUS_OK;}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntDataProvider p={NULL,rows,cell,1U};UmiUiEntCellValue v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_data_provider_cell(&p,1U,0U,&v)!=UMI_STATUS_OK||v.integer_value!=1)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_data_provider_cell(&p,2U,0U,&v)!=UMI_STATUS_NOT_FOUND)return 2;puts("ok");return 0;}
