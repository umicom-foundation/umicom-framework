/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_timeframe/test_codec.c
 * PURPOSE: Read legacy source views and reject malformed version-two timeframe metadata atomically.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include "../../src/chart/checkpoint_private.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    UmiWorkbenchLayoutDataFieldSet *fields = calloc(1U,sizeof *fields); CHECK(fields != NULL);
    const char *legacy = "scope=local\npane=NQ\nrevision=1\nsaved_at_ms=1000\nsource_revision=2\ncount=0\nvisible_bars=8\nanchor=300000\npinned=1\n";
    char text[UMI_CHART_CHECKPOINT_VALUE];
    ChartCheckpointMetadata output, before; memset(&output,0x5a,sizeof output); memcpy(&before,&output,sizeof before);
    if (strcmp(name,"roundtrip")==0) {
        ChartCheckpointMetadata input = {0}; strcpy(input.scope,"local"); strcpy(input.summary.pane_id,"NQ");
        input.storageRevision=1U; input.summary.navigation=(UmiChartNavigation){6U,INT64_MIN,1,300000U};
        OK(UmiChartCheckpointEncodeMetadata(fields,&input,text)); OK(UmiChartCheckpointDecodeMetadata(fields,text,&output));
        CHECK(output.summary.navigation.interval_ms==300000U && output.summary.navigation.anchor_ms==INT64_MIN && output.summary.navigation.visible_bars==6U);
    } else {
        const char *extra = "";
        if (strcmp(name,"legacy")==0) extra="";
        else if (strcmp(name,"version")==0) extra="view_version=3\ninterval_ms=300000\n";
        else if (strcmp(name,"interval")==0) extra="view_version=2\ninterval_ms=120000\n";
        else if (strcmp(name,"overflow")==0) extra="view_version=2\ninterval_ms=4294967296\n";
        else if (strcmp(name,"partial")==0) extra="view_version=2\n";
        else if (strcmp(name,"duplicate")==0) extra="view_version=2\nview_version=2\n";
        else if (strcmp(name,"unknown")==0) extra="view_version=2\nfuture=300000\n";
        else if (strcmp(name,"negative")==0) extra="view_version=2\ninterval_ms=-1\n";
        else return 2;
        (void)snprintf(text,sizeof text,"%s%s",legacy,extra);
        UmiStatus status=UmiChartCheckpointDecodeMetadata(fields,text,&output);
        if (strcmp(name,"legacy")==0) {
            CHECK(status==UMI_STATUS_OK && output.summary.navigation.interval_ms==0U && output.summary.navigation.visible_bars==8U && output.summary.navigation.anchor_ms==300000);
        } else CHECK(status==UMI_STATUS_PARSE_ERROR && memcmp(&before,&output,sizeof output)==0);
    }
    free(fields); return 0;
}
