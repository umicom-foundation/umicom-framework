/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_timeframe/test_sqlite.c
 * PURPOSE: Keep view intervals durable across database reopen and failed transaction publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc==3); FILE *reserved=fopen(argv[2],"wx"); CHECK(reserved!=NULL); CHECK(fclose(reserved)==0);
    UmiDataServer *server=NULL; UmiStatus status=umi_data_server_create_sqlite(argv[2],&server);
    if(status==UMI_STATUS_UNAVAILABLE){CHECK(remove(argv[2])==0);return 77;} OK(status);
    UmiChartNavigation navigation={6U,300000,1,300000U}; UmiChartDocument *first=NULL,*second=NULL,*loaded=NULL;
    OK(UmiChartDocumentCreate("NQ",&navigation,NULL,0U,1U,&first)); navigation.interval_ms=3600000U;
    OK(UmiChartDocumentCreate("NQ",&navigation,NULL,0U,2U,&second)); UmiChartCheckpointReport report;
    OK(UmiChartCheckpointSave(server,"timeframes",first,0U,1000U,&report)); CHECK(report.durable);
    if(strcmp(argv[1],"rollback")==0){
        OK(umi_data_server_execute(server,"CREATE TRIGGER reject_timeframe BEFORE INSERT ON umicom_kv WHEN NEW.key LIKE '%.primary.manifest' BEGIN SELECT RAISE(ABORT, 'fixture'); END;"));
        CHECK(UmiChartCheckpointSave(server,"timeframes",second,1U,2000U,&report)!=UMI_STATUS_OK);
        CHECK(!umi_data_server_in_transaction(server)); OK(umi_data_server_execute(server,"DROP TRIGGER reject_timeframe;"));
    }else{CHECK(strcmp(argv[1],"restart")==0);OK(UmiChartCheckpointSave(server,"timeframes",second,1U,2000U,&report));}
    umi_data_server_destroy(server); server=NULL; OK(umi_data_server_create_sqlite(argv[2],&server));
    OK(UmiChartCheckpointLoad(server,"timeframes","NQ",&loaded,&report)); UmiChartDocumentSummary summary;
    OK(UmiChartDocumentGetSummary(loaded,&summary));
    CHECK(summary.navigation.interval_ms==(strcmp(argv[1],"rollback")==0?300000U:3600000U));
    CHECK(summary.navigation.visible_bars==6U && summary.navigation.anchor_ms==300000 && summary.navigation.pinned);
    UmiChartDocumentDestroy(first);UmiChartDocumentDestroy(second);UmiChartDocumentDestroy(loaded);
    umi_data_server_destroy(server);CHECK(remove(argv[2])==0);return 0;
}
