/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/boot_media/report.c
 *
 * PURPOSE:
 *   Describe complete media observations through one bounded presentation contract for native clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/boot_media/media.h"
#include "umicom/base/utf8.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Format into private storage before publishing. Refused reports cannot leave
 * a truncated or stale-looking success description in the caller's buffer. */
UmiStatus UmiBootMediaReportDescribe(UmiStatus result,const UmiBootMediaReport *report,
    char *output,size_t capacity)
{
    if(!report||!output||!capacity)return UMI_STATUS_INVALID_ARGUMENT;
    if((unsigned)result>(unsigned)UMI_STATUS_BUSY||
        (unsigned)report->phase>(unsigned)UMI_BOOT_MEDIA_FINISHED)
        return UMI_STATUS_INVALID_ARGUMENT;
    if(report->status!=result)return UMI_STATUS_INVALID_STATE;
    const int flags[]={report->outputCreated,report->writeStarted,report->verified};
    for(size_t i=0;i<sizeof flags/sizeof flags[0];++i)
        if(flags[i]!=0&&flags[i]!=1)return UMI_STATUS_INVALID_ARGUMENT;
    if(report->verified&&result!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;
    if(report->bytesDone>report->bytesTotal)return UMI_STATUS_INVALID_ARGUMENT;
    const char *end=memchr(report->detail,0,sizeof report->detail);
    if(!end||!UmiUtf8Validate((const unsigned char *)report->detail,
        (size_t)(end-report->detail),NULL))return UMI_STATUS_INVALID_ARGUMENT;
    static const char *const phases[]={"Inspect","Recheck","Write","Flush","Verify","Finished"};
    char candidate[UMI_BOOT_MEDIA_REPORT_TEXT];
    int length=snprintf(candidate,sizeof candidate,
        "Status: %s (%d)\r\nPhase: %s; bytes observed: %" PRIu64 "/%" PRIu64 "\r\n"
        "Practice file created: %s\r\nPayload write attempted: %s\r\n"
        "Byte agreement verified: %s\r\nDetail: %s\r\n"
        "These observations do not establish a successful boot.",
        umi_status_text(result),(int)result,phases[report->phase],
        report->bytesDone,report->bytesTotal,report->outputCreated?"yes":"no",
        report->writeStarted?"yes":"no",report->verified?"yes":"no",report->detail);
    if(length<0)return UMI_STATUS_INTERNAL_ERROR;
    if((size_t)length>=sizeof candidate||(size_t)length>=capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(output,candidate,(size_t)length+1U);
    return UMI_STATUS_OK;
}
