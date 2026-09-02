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
/*
 * Provide the ui ent csv export field operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_csv_export_field(const char *in,char *out,size_t cap){size_t i,n=0;int quote=0;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!in||!out||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;in[i];i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(in[i]==','||in[i]=='"'||in[i]=='\n'||in[i]=='\r'){quote=1;break;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(quote){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n+1U>=cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[n++]='"';}/* Visit each bounded item once so every record receives the same rule. */ for(i=0;in[i];i++){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(in[i]=='"'){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n+2U>=cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[n++]='"';out[n++]='"';}/* Use this fallback path when the earlier condition does not apply. */ else{/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n+1U>=cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[n++]=in[i];}}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(quote){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n+1U>=cap)return UMI_STATUS_CAPACITY_EXCEEDED;out[n++]='"';}out[n]='\0';return UMI_STATUS_OK;}
