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
void umi_analytics_export_table_init(UmiAnalyticsExportTable *t,size_t columns){if(t!=NULL){memset(t,0,sizeof *t);t->columns=columns<=UMI_ANALYTICS_EXPORT_COLUMNS?columns:UMI_ANALYTICS_EXPORT_COLUMNS;}}
UmiStatus umi_analytics_export_table_set(UmiAnalyticsExportTable *t,size_t row,size_t column,const char *value){UmiStatus s;if(t==NULL||row>=UMI_ANALYTICS_EXPORT_ROWS||column>=t->columns)return UMI_STATUS_INVALID_ARGUMENT;s=umi_analytics_copy_text(t->cells[row][column],UMI_ANALYTICS_EXPORT_CELL,value);if(s!=0)return s;if(row+1U>t->rows)t->rows=row+1U;return UMI_STATUS_OK;}
static int needs_quote(const char *s){for(;*s!='\0';s++)if(*s==','||*s=='"'||*s=='\n'||*s=='\r')return 1;return 0;}
UmiStatus umi_analytics_export_table_csv(const UmiAnalyticsExportTable *t,char *buffer,size_t capacity,size_t *out_length){size_t pos=0U,r,c;if(t==NULL||buffer==NULL||out_length==NULL||capacity==0U)return UMI_STATUS_INVALID_ARGUMENT;buffer[0]='\0';for(r=0;r<t->rows;r++){for(c=0;c<t->columns;c++){const char *s=t->cells[r][c];int quote=needs_quote(s);if(c>0U){if(pos+1U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;buffer[pos++]=',';}if(quote){if(pos+1U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;buffer[pos++]='"';}while(*s!='\0'){if(*s=='"'){if(pos+2U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;buffer[pos++]='"';buffer[pos++]='"';s++;}else{if(pos+1U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;buffer[pos++]=*s++;}}if(quote){if(pos+1U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;buffer[pos++]='"';}}if(pos+1U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;buffer[pos++]='\n';}buffer[pos]='\0';*out_length=pos;return UMI_STATUS_OK;}
