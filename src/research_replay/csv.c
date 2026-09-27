/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "umicom/strategy_research/research_csv.h"
#include <errno.h>
#include <locale.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
static int Unsigned(const char *s,uint64_t max,uint64_t *out)
{
    uint64_t n=0;if(*s==0) return 0;
    while(*s) {
        if(*s<'0'||*s>'9') return 0;
        unsigned d=(unsigned)(*s-'0');
        if(n>(max-d)/10U) return 0;
        n=n*10U+d;s++;
    }
    *out=n;return 1;
}
static int Number(const char *s,double *out)
{
    if(*s==0) return 0;
    for(const char *p=s;*p;p++)
        if(!((*p>='0'&&*p<='9')||*p=='.'||*p=='e'||*p=='E'||*p=='+'||*p=='-')) return 0;
    char *end=NULL;errno=0;double v=strtod(s,&end);
    if(end==s||*end!=0||errno==ERANGE||!isfinite(v)) return 0;
    *out=v;return 1;
}
UmiStatus UmiResearchParseCsv(const char *text,size_t bytes,const UmiInstrument *instrument,
    UmiResearchObservation **outEvents,size_t *outCount,size_t *outErrorLine)
{
    if(outEvents==NULL||outCount==NULL||outErrorLine==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outEvents=NULL;*outCount=0;*outErrorLine=0;
    if(text==NULL||instrument==NULL||bytes==0) return UMI_STATUS_INVALID_ARGUMENT;
    if(bytes>UMI_RESEARCH_CSV_BYTE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(strcmp(localeconv()->decimal_point,".")!=0) return UMI_STATUS_UNAVAILABLE;
    if(memchr(text,0,bytes)!=NULL) return UMI_STATUS_PARSE_ERROR;
    size_t lines=0;for(size_t n=0;n<bytes;n++) if(text[n]=='\n') lines++;
    if(text[bytes-1]!='\n') lines++;
    if(lines<2) return UMI_STATUS_PARSE_ERROR;
    if(lines-1>UMI_RESEARCH_OBSERVATION_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiResearchObservation *events=calloc(lines-1,sizeof *events);
    if(events==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t offset=0,row=0,count=0;UmiStatus status=UMI_STATUS_OK;
    while(offset<bytes) {
        row++;size_t start=offset;while(offset<bytes&&text[offset]!='\n') offset++;
        size_t length=offset-start;if(offset<bytes) offset++;
        if(length&&text[start+length-1]=='\r') length--;
        if(length==0||length>=512) {status=UMI_STATUS_PARSE_ERROR;break;}
        char line[512];memcpy(line,text+start,length);line[length]=0;
        if(row==1) {
            if(strcmp(line,"sequence,time_ms,bid,ask,bid_size,ask_size")!=0) {status=UMI_STATUS_PARSE_ERROR;break;}
            continue;
        }
        char *fields[6]={line};size_t col=1;
        for(size_t n=0;n<length;n++) if(line[n]==',') {
            line[n]=0;if(col==6) {status=UMI_STATUS_PARSE_ERROR;break;} fields[col++]=line+n+1;
        }
        if(status!=UMI_STATUS_OK||col!=6) {status=UMI_STATUS_PARSE_ERROR;break;}
        UmiResearchObservation *e=&events[count];uint64_t timestamp=0;
        if(!Unsigned(fields[0],UINT64_MAX,&e->sequence)||!Unsigned(fields[1],INT64_MAX,&timestamp)||
            !Number(fields[2],&e->quote.bid)||!Number(fields[3],&e->quote.ask)||
            !Number(fields[4],&e->quote.bid_size)||!Number(fields[5],&e->quote.ask_size)) {
            status=UMI_STATUS_PARSE_ERROR;break;
        }
        e->quote.event_time_ms=(int64_t)timestamp;e->quote.instrument=*instrument;count++;
    }
    if(status!=UMI_STATUS_OK) {free(events);*outErrorLine=row;return status;}
    *outEvents=events;*outCount=count;return UMI_STATUS_OK;
}
