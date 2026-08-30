/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/csv_export.c
 *
 * PURPOSE:
 *   Implement bounded CSV field escaping.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/csv_export.h"
#include <string.h>
UmiStatus umi_ui_ent_csv_export_field(const char *in,char *out,size_t cap){size_t i,n=0;int quote=0;if(!in||!out||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;in[i];i++)if(in[i]==','||in[i]=='"'||in[i]=='\n'||in[i]=='\r'){quote=1;break;}if(quote){if(n+1U>=cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[n++]='"';}for(i=0;in[i];i++){if(in[i]=='"'){if(n+2U>=cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[n++]='"';out[n++]='"';}else{if(n+1U>=cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[n++]=in[i];}}if(quote){if(n+1U>=cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[n++]='"';}out[n]='\0';return UMI_STATUS_OK;}
