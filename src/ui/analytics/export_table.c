/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/export_table.c
 *
 * PURPOSE:
 *   Build bounded analytical tables and emit standards-compliant CSV text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/export_table.h"

#include <stdio.h>
#include <string.h>
/*
 * Initialise analytics export table from caller-provided values so later operations
 * receive a known state.
 */
void umi_analytics_export_table_init(UmiAnalyticsExportTable *t,size_t columns){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t!=NULL){memset(t,0,sizeof *t);t->columns=columns<=UMI_ANALYTICS_EXPORT_COLUMNS?columns:UMI_ANALYTICS_EXPORT_COLUMNS;}}
/*
 * Copy analytics export table into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_analytics_export_table_set(UmiAnalyticsExportTable *t,size_t row,size_t column,const char *value){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||row>=UMI_ANALYTICS_EXPORT_ROWS||column>=t->columns)return UMI_STATUS_INVALID_ARGUMENT;s=umi_analytics_copy_text(t->cells[row][column],UMI_ANALYTICS_EXPORT_CELL,value);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(row+1U>t->rows)t->rows=row+1U;return UMI_STATUS_OK;}
/* Provide the needs quote operation used by this module and its client applications. */
static int needs_quote(const char *s){/* Visit each bounded item once so every record receives the same rule. */ for(;*s!='\0';s++)/* Apply this branch only when its contract condition is satisfied. */ if(*s==','||*s=='"'||*s=='\n'||*s=='\r')return 1;return 0;}
/*
 * Provide the analytics export table csv operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_export_table_csv(const UmiAnalyticsExportTable *t,char *buffer,size_t capacity,size_t *out_length){size_t pos=0U,r,c;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||buffer==NULL||out_length==NULL||capacity==0U)return UMI_STATUS_INVALID_ARGUMENT;buffer[0]='\0';/* Visit each bounded item once so every record receives the same rule. */ for(r=0;r<t->rows;r++){/* Visit each bounded item once so every record receives the same rule. */ for(c=0;c<t->columns;c++){const char *s=t->cells[r][c];int quote=needs_quote(s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c>0U){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pos+1U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;buffer[pos++]=',';}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(quote){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pos+1U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;buffer[pos++]='"';}/* Continue only while work remains available; the loop body advances the state on each pass. */ while(*s!='\0'){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(*s=='"'){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pos+2U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;buffer[pos++]='"';buffer[pos++]='"';s++;}/* Use this fallback path when the earlier condition does not apply. */ else{/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pos+1U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;buffer[pos++]=*s++;}}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(quote){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pos+1U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;buffer[pos++]='"';}}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pos+1U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;buffer[pos++]='\n';}buffer[pos]='\0';*out_length=pos;return UMI_STATUS_OK;}
