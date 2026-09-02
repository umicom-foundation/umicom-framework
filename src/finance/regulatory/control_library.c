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
/*
 * Initialise reg control library from caller-provided values so later operations receive a
 * known state.
 */
void umi_reg_control_library_init(UmiControlLibrary *l){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(l!=NULL)memset(l,0,sizeof *l); }
/*
 * Provide the reg control library contains operation used by this module and its client
 * applications.
 */
int umi_reg_control_library_contains(const UmiControlLibrary *l,const char *id){ size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||id==NULL)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<l->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(l->control_ids[i],id)==0)return 1;return 0; }
/* Add reg control library only after its inputs and available capacity have been checked. */
UmiStatus umi_reg_control_library_add(UmiControlLibrary *l,const char *id){ UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l->count>=UMI_REG_MAX_RULES)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_reg_control_library_contains(l,id))return UMI_STATUS_ALREADY_EXISTS;s=umi_reg_copy_text(l->control_ids[l->count],UMI_REG_ID_CAPACITY,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)++l->count;return s; }
