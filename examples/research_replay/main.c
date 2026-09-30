/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/research_replay/main.c
 * PURPOSE:
 *   Expose research replay commands without submitting live orders.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "strategy.h"
#include "umicom/strategy_research/research_csv.h"
#include <inttypes.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
static int Print(UmiResearchReplay *r)
{
    UmiResearchSnapshot s;
    if(UmiResearchReplaySnapshot(r,&s)!=UMI_STATUS_OK) return 1;
    puts("UMICOM RESEARCH REPLAY - simulation only, no broker");
    printf("Input/configuration SHA-256: %s\n",s.inputFingerprint);
    puts("sequence,time_ms,position,fill_units,fill_price,commission,equity,pending,target");
    for(size_t n=0;n<s.processed;n++) {
        UmiResearchTrace t;
        if(UmiResearchReplayTraceAt(r,n,&t)!=UMI_STATUS_OK) return 1;
        printf("%" PRIu64 ",%" PRId64 ",%d,%.6f,%.6f,%.6f,%.6f,%d,%d\n",
            t.sequence,t.timeMilliseconds,t.position,t.signedFillUnits,t.fillPrice,
            t.stepCommission,t.equity,t.hasPending,t.pendingTarget);
    }
    const char *states[]={"READY","RUNNING","COMPLETED_INPUT","CANCELLED","FAILED"};
    printf("State: %s; processed: %zu/%zu; closed trades: %" PRIu64 "\n",
        states[(int)s.state],s.processed,s.total,s.closedTrades.tradeCount);
    printf("Closed net P&L: %.6f; marked equity: %.6f; all paid fees: %.6f\n",
        s.closedTrades.netProfit,s.equity,s.totalCommission);
    printf("Marked drawdown: %.6f (%.6f%%); open direction: %d; pending: %d\n",
        s.markToMarketDrawdown,s.markToMarketDrawdownPercent,s.position,s.hasPending);
    if(s.profitFactorAvailable) printf("Closed profit factor: %.6f\n",s.closedTrades.profitFactor);
    else puts("Closed profit factor: unavailable (no losing closed trades).");
    puts("No end-of-data liquidation is invented.");
    return ferror(stdout)?1:0;
}
static UmiStatus Load(const char *path,UmiResearchObservation **events,size_t *count)
{
    struct stat info;
    if(stat(path,&info)!=0||!S_ISREG(info.st_mode)||info.st_size<=0||
        (uint64_t)info.st_size>UMI_RESEARCH_CSV_BYTE_LIMIT) return UMI_STATUS_IO_ERROR;
    FILE *f=fopen(path,"rb");if(f==NULL) return UMI_STATUS_IO_ERROR;
    size_t size=(size_t)info.st_size;char *text=malloc(size+1);
    if(text==NULL) {fclose(f);return UMI_STATUS_OUT_OF_MEMORY;}
    size_t got=fread(text,1,size,f);int extra=fgetc(f);int bad=ferror(f);
    int closed=fclose(f);
    UmiStatus s=UMI_STATUS_IO_ERROR;size_t line=0;
    UmiInstrument instrument=PracticeInstrument();
    /* Imported values retain an explicit generic practice identity. This is not
     * an instrument-master lookup, licensed data loader or an authenticated feed. */
    if(got==size&&extra==EOF&&!bad&&closed==0)
        s=UmiResearchParseCsv(text,size,&instrument,events,count,&line);
    if(s!=UMI_STATUS_OK) fprintf(stderr,"Input refused; line %zu: %s\n",line,umi_status_text(s));
    free(text);return s;
}
int main(int argc,char **argv)
{
    (void)setlocale(LC_NUMERIC,"C");
    if(argc==2&&strcmp(argv[1],"--help")==0) {
        puts("umicom-research-replay demo | --self-test | csv FILE | sweep");
        puts("Six-column CSV uses sequence,time_ms,bid,ask,bid_size,ask_size. stdout only; no report file is overwritten.");
        return 0;
    }
    int demo=argc==2&&(strcmp(argv[1],"demo")==0||strcmp(argv[1],"--self-test")==0);
    int sweep=argc==2&&strcmp(argv[1],"sweep")==0;
    int csv=argc==3&&strcmp(argv[1],"csv")==0;
    if(!demo&&!sweep&&!csv) {fprintf(stderr,"Use --help for the supported commands.\n");return 2;}
    UmiResearchObservation sample[8];PracticeQuotes(sample);
    UmiResearchObservation *events=sample,*allocated=NULL;size_t count=8;
    if(csv) {
        if(Load(argv[2],&allocated,&count)!=UMI_STATUS_OK) return 1;
        events=allocated;
    }
    int result=0;size_t candidates=sweep?3U:1U;
    for(size_t n=0;n<candidates;n++) {
        PracticeThresholds thresholds={100,102};UmiResearchConfig c=UmiResearchConfigDefault();
        c.latencyMilliseconds=(uint32_t)n*100U;
        snprintf(c.strategyTag,sizeof c.strategyTag,"example-thresholds-100-102");
        UmiResearchReplay *r=NULL;
        UmiStatus status=UmiResearchReplayCreate(&c,events,count,PracticeStrategy,&thresholds,&r);
        if(status==UMI_STATUS_OK) {size_t done=0;status=UmiResearchReplayRun(r,count,&done);}
        if(status==UMI_STATUS_OK && argc==2 && strcmp(argv[1],"--self-test")==0) {
            UmiResearchSnapshot checked;
            status=UmiResearchReplaySnapshot(r,&checked);
            if(status==UMI_STATUS_OK && (checked.closedTrades.tradeCount!=2 ||
                fabs(checked.closedTrades.netProfit-4.6)>1e-8 || checked.position!=0 || checked.hasPending))
                status=UMI_STATUS_INTERNAL_ERROR;
        }
        if(status==UMI_STATUS_OK) {
            printf("Latency candidate: %u ms\n",c.latencyMilliseconds);
            result|=Print(r);
        } else {fprintf(stderr,"Research failed: %s\n",umi_status_text(status));result=1;}
        UmiResearchReplayDestroy(r);
    }
    free(allocated);return result;
}
