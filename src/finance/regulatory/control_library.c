/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/control_library.c
 *
 * PURPOSE:
 *   Maintain a bounded catalogue of unique financial-control identifiers.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/control_library.h"

#include <string.h>
void umi_reg_control_library_init(UmiControlLibrary *l){ if(l!=NULL)memset(l,0,sizeof *l); }
int umi_reg_control_library_contains(const UmiControlLibrary *l,const char *id){ size_t i;if(l==NULL||id==NULL)return 0;for(i=0U;i<l->count;++i)if(strcmp(l->control_ids[i],id)==0)return 1;return 0; }
UmiStatus umi_reg_control_library_add(UmiControlLibrary *l,const char *id){ UmiStatus s;if(l==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(l->count>=UMI_REG_MAX_RULES)return UMI_STATUS_CAPACITY_EXCEEDED;if(umi_reg_control_library_contains(l,id))return UMI_STATUS_ALREADY_EXISTS;s=umi_reg_copy_text(l->control_ids[l->count],UMI_REG_ID_CAPACITY,id);if(s==UMI_STATUS_OK)++l->count;return s; }
