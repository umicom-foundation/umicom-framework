/*-----------------------------------------------------------------------------
 * Umicom Framework tests
 * File: tests/os_foundation/test_boot_report.c
 *
 * PURPOSE:
 *   Exercise boot evidence grammar and state invariants, including hostile bytes.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/boot_report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(e) do { if (!(e)) { fprintf(stderr, "check failed at %d: %s\n", __LINE__, #e); return 1; } } while (0)
static const char SOURCE[] = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
static void Report(char *text, const char *mode, const char *state, unsigned planned, unsigned completed, const char *reason)
{
    (void)snprintf(text, 1024, "UMICOM_BOOT_REPORT 1\nmode=%s\nstate=%s\nplanned=%u\ncompleted=%u\nreason=%s\nsource=%s\n", mode, state, planned, completed, reason, SOURCE);
}
static int Parse(const char *text)
{
    UmiBootReport report;
    return UmiBootReportParse(text, strlen(text), &report) == UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    char text[1024]; UmiBootReport result;
    if (argc != 2) return 2;
    const char *test = argv[1]; Report(text, "normal", "ready", 2, 2, "none");
    if (strcmp(test, "ready") == 0) {
        CHECK(UmiBootReportParse(text, strlen(text), &result) == UMI_STATUS_OK);
        CHECK(result.completed == 2 && result.planned == 2 && result.mode == UMI_BOOT_REPORT_NORMAL);
        CHECK(strcmp(result.sourceId, SOURCE) == 0 && strcmp(UmiBootReportStateText(result.state), "ready") == 0);
    } else if (strcmp(test, "starting") == 0) {
        Report(text, "normal", "starting", 2, 1, "none"); CHECK(Parse(text));
    } else if (strcmp(test, "requested") == 0) {
        Report(text, "recovery", "recovery", 2, 0, "requested"); CHECK(Parse(text));
    } else if (strcmp(test, "failure") == 0) {
        const char *reasons[] = {"configuration","mount","service-exit","service-timeout","service-launch","report-write","privilege"};
        for (size_t i=0; i<sizeof reasons/sizeof reasons[0]; ++i) {
            Report(text,"normal","recovery",2,1,reasons[i]); CHECK(Parse(text));
        }
    } else if (strcmp(test, "order") == 0) {
        (void)snprintf(text,sizeof text,"UMICOM_BOOT_REPORT 1\nsource=%s\nreason=none\ncompleted=2\nplanned=2\nstate=ready\nmode=normal\n",SOURCE); CHECK(Parse(text));
    } else if (strcmp(test, "duplicate") == 0) { strcat(text,"mode=normal\n"); CHECK(!Parse(text));
    } else if (strcmp(test, "missing") == 0) { char *p=strstr(text,"source="); CHECK(p); *p=0; CHECK(!Parse(text));
    } else if (strcmp(test, "version") == 0) { text[19-1]='2'; CHECK(!Parse(text));
    } else if (strcmp(test, "count") == 0) {
        Report(text,"normal","ready",17,17,"none"); CHECK(!Parse(text));
        Report(text,"normal","starting",2,3,"none"); CHECK(!Parse(text));
        Report(text,"normal","ready",0,0,"none"); CHECK(!Parse(text));
    } else if (strcmp(test, "source") == 0) { char *p=strstr(text,"source="); CHECK(p); p[7]='g'; CHECK(!Parse(text));
    } else if (strcmp(test, "control") == 0) { size_t n=strlen(text); text[22]=0; CHECK(UmiBootReportParse(text,n,&result)!=UMI_STATUS_OK);
    } else if (strcmp(test, "unknown") == 0) { strcat(text,"execute=/bin/sh\n"); CHECK(!Parse(text));
    } else if (strcmp(test, "unchanged") == 0) {
        memset(&result,0xA5,sizeof result); UmiBootReport old=result;
        CHECK(UmiBootReportParse("broken",6,&result)!=UMI_STATUS_OK); CHECK(memcmp(&result,&old,sizeof result)==0);
        CHECK(UmiBootReportParse(NULL,0,&result)!=UMI_STATUS_OK); CHECK(UmiBootReportParse(text,strlen(text),NULL)!=UMI_STATUS_OK);
    } else if (strcmp(test, "truncation") == 0) {
        for(size_t n=0;n<strlen(text);++n) CHECK(UmiBootReportParse(text,n,&result)!=UMI_STATUS_OK);
    } else if (strcmp(test, "states") == 0) {
        Report(text,"normal","ready",2,1,"none"); CHECK(!Parse(text));
        Report(text,"recovery","ready",2,2,"none"); CHECK(!Parse(text));
        Report(text,"normal","recovery",2,0,"none"); CHECK(!Parse(text));
        Report(text,"normal","recovery",2,0,"requested"); CHECK(!Parse(text));
        Report(text,"recovery","recovery",2,1,"requested"); CHECK(!Parse(text));
        CHECK(strcmp(UmiBootReportStateText((UmiBootReportState)99),"unknown")==0);
    } else if (strcmp(test, "fuzz") == 0) {
        uint32_t random=1234567;
        for (size_t run=0;run<10000;++run) {
            Report(text,"normal","ready",2,2,"none"); size_t n=strlen(text);
            random=random*1664525U+1013904223U; size_t position=random%n;
            random=random*1664525U+1013904223U; text[position]=(char)(random&255U);
            UmiStatus status=UmiBootReportParse(text,n,&result);
            if(status==UMI_STATUS_OK) CHECK(result.completed<=result.planned && result.planned<=16U);
        }
    } else return 2;
    return 0;
}
