/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_connection.c
 * PURPOSE:
 *   Check broker connection state, protocol validation and bounded observations.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "fixture.h"
#include <math.h>
#include <float.h>
#include <limits.h>

static int Policy(const char *name)
{
    UmiIbkrConnectionOptions o=UmiIbkrConnectionOptionsDefault();
    if(!strcmp(name,"policy_defaults")){
        CHECK(o.environment==UMI_TRADING_PAPER&&o.adapter.readOnly==1&&o.adapter.paperOnly==1);
        CHECK(UmiIbkrConnectionValidate(&o)==UMI_STATUS_OK);
    }else if(!strcmp(name,"policy_ports")){
        CHECK(UmiIbkrDefaultPort(UMI_IBKR_TWS,UMI_TRADING_PAPER)==7497U);
        CHECK(UmiIbkrDefaultPort(UMI_IBKR_TWS,UMI_TRADING_LIVE)==7496U);
        CHECK(UmiIbkrDefaultPort(UMI_IBKR_GATEWAY,UMI_TRADING_PAPER)==4002U);
        CHECK(UmiIbkrDefaultPort(UMI_IBKR_GATEWAY,UMI_TRADING_LIVE)==4001U);
        CHECK(UmiIbkrDefaultPort((UmiIbkrProgram)99,UMI_TRADING_PAPER)==0);
    }else if(!strcmp(name,"policy_live_ack")){
        o.environment=UMI_TRADING_LIVE;o.adapter.paperOnly=0;
        CHECK(UmiIbkrConnectionValidate(&o)==UMI_STATUS_PERMISSION_DENIED);
        o.acknowledgeLive=true;CHECK(UmiIbkrConnectionValidate(&o)==UMI_STATUS_OK);
    }else if(!strcmp(name,"policy_no_remote")){
        strcpy(o.adapter.host,"192.0.2.1");CHECK(UmiIbkrConnectionValidate(&o)==UMI_STATUS_PERMISSION_DENIED);
    }else if(!strcmp(name,"policy_read_only")){
        o.adapter.readOnly=0;CHECK(UmiIbkrConnectionValidate(&o)==UMI_STATUS_PERMISSION_DENIED);
    }else if(!strcmp(name,"policy_client_zero")){
        o.adapter.clientId=0;CHECK(UmiIbkrConnectionValidate(&o)==UMI_STATUS_INVALID_ARGUMENT);
    }else if(!strcmp(name,"policy_invalid_mode")){
        o.environment=UMI_TRADING_SIMULATION;CHECK(UmiIbkrConnectionValidate(&o)==UMI_STATUS_INVALID_ARGUMENT);
    }else if(!strcmp(name,"policy_timeout")){
        o.timeoutMilliseconds=0;CHECK(UmiIbkrConnectionValidate(&o)==UMI_STATUS_INVALID_ARGUMENT);
        o.timeoutMilliseconds=60001U;CHECK(UmiIbkrConnectionValidate(&o)==UMI_STATUS_INVALID_ARGUMENT);
    }else if(!strcmp(name,"policy_account_before_connection")){
        strcpy(o.adapter.account,"DU123");CHECK(UmiIbkrConnectionValidate(&o)==UMI_STATUS_INVALID_ARGUMENT);
    }else if(!strcmp(name,"policy_no_port_attestation")){
        o.adapter.port=7496U;CHECK(UmiIbkrConnectionValidate(&o)==UMI_STATUS_OK);
        /* Custom port is allowed; even a conventional LIVE port does not turn
         * this requested PAPER label into evidence of a server environment. */
    }else return 1;
    return 0;
}
static int Mapping(const char *name)
{
    UmiIbkrAdapterConfig config;umi_ibkr_adapter_config_init(&config);
    UmiOrderRequest request={0};request.quantity=1;request.side=UMI_SIDE_BUY;
    request.environment=UMI_TRADING_PAPER;request.type=UMI_ORDER_MARKET;request.tif=UMI_TIF_DAY;
    UmiIbkrOrderMessage out,before;memset(&out,0x5a,sizeof out);before=out;
    UmiStatus expected=UMI_STATUS_INVALID_ARGUMENT;
    if(!strcmp(name,"mapping_nan_quantity"))request.quantity=NAN;
    else if(!strcmp(name,"mapping_infinite_quantity"))request.quantity=INFINITY;
    else if(!strcmp(name,"mapping_invalid_side"))request.side=(UmiSide)0;
    else if(!strcmp(name,"mapping_invalid_mode"))request.environment=(UmiTradingEnvironment)90;
    else if(!strcmp(name,"mapping_nan_price"))request.limit_price=NAN;
    else if(!strcmp(name,"mapping_limit_required"))request.type=UMI_ORDER_LIMIT;
    else if(!strcmp(name,"mapping_stop_required"))request.type=UMI_ORDER_STOP;
    else if(!strcmp(name,"mapping_policy_bool"))config.paperOnly=2;
    else if(!strcmp(name,"mapping_unterminated_host"))memset(config.host,'x',sizeof config.host);
    else if(!strcmp(name,"mapping_unterminated_account"))memset(config.account,'x',sizeof config.account);
    else if(!strcmp(name,"mapping_read_only")){config.readOnly=1;expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(!strcmp(name,"mapping_paper_live")){request.environment=UMI_TRADING_LIVE;expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(!strcmp(name,"mapping_valid")){
        request.side=UMI_SIDE_SELL;request.type=UMI_ORDER_STOP_LIMIT;request.limit_price=99;request.stop_price=100;
        CHECK(umi_ibkr_adapter_map_order(&request,&config,&out)==UMI_STATUS_OK);
        CHECK(!strcmp(out.action,"SELL")&&!strcmp(out.orderType,"STP LMT")&&out.transmit==1);
        return 0;
    }else return 1;
    CHECK(umi_ibkr_adapter_map_order(&request,&config,&out)==expected);
    CHECK(memcmp(&out,&before,sizeof out)==0);return 0;
}
static int Wire(Fixture *f,const char *name)
{
    if(!strcmp(name,"wire_open_once")){
        CHECK(UmiIbkrConnectionOpen(f->c,0)==UMI_STATUS_OK);
        CHECK(UmiIbkrConnectionOpen(f->c,0)==UMI_STATUS_INVALID_STATE);
        UmiIbkrConnectionClose(f->c);UmiIbkrConnectionClose(f->c);CHECK(f->closes==1U);
        CHECK(UmiIbkrConnectionOpen(f->c,0)==UMI_STATUS_INVALID_STATE);return 0;
    }
    if(!strcmp(name,"wire_connect_timeout")){
        f->readyStatus=UMI_STATUS_BUSY;CHECK(UmiIbkrConnectionOpen(f->c,0)==UMI_STATUS_OK);
        CHECK(UmiIbkrConnectionPump(f->c,10000)==UMI_STATUS_TIMEOUT);CHECK(f->closes==1U);return 0;
    }
    if(!strcmp(name,"wire_write_failure")){
        f->writeStatus=UMI_STATUS_IO_ERROR;CHECK(UmiIbkrConnectionOpen(f->c,0)==UMI_STATUS_OK);
        CHECK(UmiIbkrConnectionPump(f->c,1)==UMI_STATUS_IO_ERROR);CHECK(f->closes==1U);return 0;
    }
    if(!strcmp(name,"wire_zero_write")){
        f->zeroWrite=true;CHECK(UmiIbkrConnectionOpen(f->c,0)==UMI_STATUS_OK);
        CHECK(UmiIbkrConnectionPump(f->c,1)==UMI_STATUS_IO_ERROR);return 0;
    }
    if(!strcmp(name,"wire_partial_write")){
        f->writeStep=1;CHECK(UmiIbkrConnectionOpen(f->c,0)==UMI_STATUS_OK);
        for(uint64_t i=0;i<17U;++i)CHECK(UmiIbkrConnectionPump(f->c,i)==UMI_STATUS_OK);
        CHECK(f->outSize==17U&&f->c->txSize==0);CHECK(!memcmp(f->output,"API\0",4));return 0;
    }
    if(!strcmp(name,"wire_handshake_version")){
        CHECK(UmiIbkrConnectionOpen(f->c,0)==UMI_STATUS_OK);CHECK(UmiIbkrConnectionPump(f->c,0)==UMI_STATUS_OK);
        FEED(f,"177","time");CHECK(UmiIbkrConnectionPump(f->c,1)==UMI_STATUS_UNAVAILABLE);
        CHECK(f->c->snapshot.state==UMI_IBKR_FAILED);return 0;
    }
    if(!strcmp(name,"wire_handshake_timeout")){
        CHECK(UmiIbkrConnectionOpen(f->c,0)==UMI_STATUS_OK);CHECK(UmiIbkrConnectionPump(f->c,0)==UMI_STATUS_OK);
        CHECK(UmiIbkrConnectionPump(f->c,10000)==UMI_STATUS_TIMEOUT);return 0;
    }
    if(!strcmp(name,"wire_before_ready")){
        CHECK(UmiIbkrConnectionReadAccount(f->c,"DU123",0)==UMI_STATUS_INVALID_STATE);return 0;
    }
    if(!strcmp(name,"wire_fragmented"))f->readStep=1;
    CHECK(Connect(f)==0);
    if(!strcmp(name,"wire_fragmented")||!strcmp(name,"wire_ready")){
        CHECK(f->c->snapshot.accountCount==2U);CHECK(!f->c->snapshot.environmentAttested&&f->c->snapshot.readOnly);return 0;
    }
    if(!strcmp(name,"wire_accounts_trailing_comma")){
        FEED(f,"15","1","DU123,DU456,");CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_OK);return 0;
    }
    if(!strcmp(name,"wire_accounts_empty_interior")){
        FEED(f,"15","1","DU123,,DU456");CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_PARSE_ERROR);return 0;
    }
    if(!strcmp(name,"wire_long_diagnostic")){
        char text[502];memset(text,'x',sizeof text-1U);text[sizeof text-1U]=0;
        FEED(f,"4","2","-1","2104",text,"");CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_OK);
        CHECK(strstr(f->c->snapshot.message,"text omitted")!=NULL);return 0;
    }
    if(!strcmp(name,"wire_account_capacity")){
        char text[512]={0};size_t at=0;
        for(unsigned i=0;i<33U;++i){int n=snprintf(text+at,sizeof text-at,"%sDU%u",i?",":"",i);CHECK(n>0);at+=(size_t)n;}
        FEED(f,"15","1",text);CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_CAPACITY_EXCEEDED);return 0;
    }
    if(!strcmp(name,"wire_position_capacity")){
        CHECK(UmiIbkrConnectionReadAccount(f->c,"DU123",10)==UMI_STATUS_OK);
        for(unsigned i=0;i<65U;++i){char id[20];(void)snprintf(id,sizeof id,"%u",i+1U);
            FEED(f,"61","3","DU123",id,"WORKSHOP","STK","","0","","","SMART","GBP","WORKSHOP","WORKSHOP","2","10");
            UmiStatus st=UmiIbkrConnectionPump(f->c,11U+i);
            CHECK(st==(i<64U?UMI_STATUS_OK:UMI_STATUS_CAPACITY_EXCEEDED));}
        CHECK(f->c->snapshot.positionCount==64U&&!f->c->snapshot.positionsComplete&&f->c->snapshot.stale);return 0;
    }
    if(!strcmp(name,"wire_snapshot_independence")){
        UmiIbkrConnectionSnapshot *copy=malloc(sizeof *copy);CHECK(copy);
        CHECK(UmiIbkrConnectionCopy(f->c,copy)==UMI_STATUS_OK);
        strcpy(copy->accounts[0],"CHANGED");CHECK(!strcmp(f->c->snapshot.accounts[0],"DU123"));free(copy);return 0;
    }
    if(!strcmp(name,"wire_frame_budget")){
        uint64_t before=f->c->snapshot.framesReceived;
        for(unsigned i=0;i<100U;++i){FEED(f,"999","ignored");}
        CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_OK);
        CHECK(f->c->snapshot.framesReceived-before==64U);
        CHECK(UmiIbkrConnectionPump(f->c,11)==UMI_STATUS_OK);
        CHECK(f->c->snapshot.framesReceived-before==100U);return 0;
    }
    if(!strcmp(name,"wire_queue_atomic")){
        f->c->txSize=UMI_IBKR_TX_LIMIT-1U;size_t before=f->c->txSize;
        CHECK(UmiIbkrConnectionReadAccount(f->c,"DU123",10)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(f->c->txSize==before&&!f->c->snapshot.requestIssued);return 0;
    }
    if(!strcmp(name,"wire_unlisted_account")){
        CHECK(UmiIbkrConnectionReadAccount(f->c,"DU999",10)==UMI_STATUS_PERMISSION_DENIED);CHECK(!f->c->snapshot.requestIssued);return 0;
    }
    if(!strcmp(name,"wire_backwards_time")){
        CHECK(UmiIbkrConnectionPump(f->c,4)==UMI_STATUS_INVALID_ARGUMENT);CHECK(f->c->snapshot.state==UMI_IBKR_READY);return 0;
    }
    if(!strcmp(name,"wire_disconnect_stale")){
        UmiIbkrConnectionClose(f->c);CHECK(f->c->snapshot.stale&&f->c->snapshot.accountCount==2U);CHECK(f->closes==1U);return 0;
    }
    if(!strcmp(name,"wire_eof")){
        f->eof=true;CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_IO_ERROR);CHECK(f->c->snapshot.stale);return 0;
    }
    if(!strcmp(name,"wire_unknown")){
        FEED(f,"999","unknown","payload");CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_OK);CHECK(f->c->snapshot.ignoredFrames==1U);return 0;
    }
    if(!strcmp(name,"wire_bad_length")){
        unsigned char bad[]={0,1,0,1};memcpy(f->input+f->inSize,bad,4);f->inSize+=4;
        CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_CAPACITY_EXCEEDED);return 0;
    }
    if(!strcmp(name,"wire_zero_length")){
        memset(f->input+f->inSize,0,4);f->inSize+=4;CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_CAPACITY_EXCEEDED);return 0;
    }
    if(!strcmp(name,"wire_missing_terminator")){
        const unsigned char bad[]={0,0,0,2,'4','9'};memcpy(f->input+f->inSize,bad,sizeof bad);f->inSize+=sizeof bad;
        CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_PARSE_ERROR);return 0;
    }
    if(!strcmp(name,"wire_farm_message")){
        FEED(f,"4","2","-1","2104","Market data farm is connected","");CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_OK);
        CHECK(f->c->snapshot.providerCode==2104&&f->c->snapshot.state==UMI_IBKR_READY);return 0;
    }
    if(!strcmp(name,"wire_backend_lost")){
        FEED(f,"4","2","-1","1100","Connectivity lost","");CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_UNAVAILABLE);
        CHECK(f->c->snapshot.stale&&f->closes==1U);return 0;
    }
    if(!strcmp(name,"wire_heartbeat_timeout")){
        CHECK(UmiIbkrConnectionPump(f->c,30005)==UMI_STATUS_OK);CHECK(f->c->pingPending);
        CHECK(UmiIbkrConnectionPump(f->c,40005)==UMI_STATUS_TIMEOUT);return 0;
    }
    if(!strcmp(name,"wire_accounts_changed")){
        FEED(f,"15","1","DU999");CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_PERMISSION_DENIED);CHECK(f->c->snapshot.accountCount==2);return 0;
    }
    if(!strcmp(name,"wire_accounts_reordered")){
        FEED(f,"15","1","DU456,DU123");CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_OK);return 0;
    }
    if(!strcmp(name,"wire_accounts_duplicate")){
        FEED(f,"15","1","DU123,DU123");CHECK(UmiIbkrConnectionPump(f->c,10)==UMI_STATUS_PARSE_ERROR);return 0;
    }
    if(!strcmp(name,"wire_outbound_allowlist")){
        const char *order[]={"3","1"};CHECK(UmiIbkrQueueFields(f->c,order,2)==UMI_STATUS_PERMISSION_DENIED);
        const char *cancel[]={"4","1"};CHECK(UmiIbkrQueueFields(f->c,cancel,2)==UMI_STATUS_PERMISSION_DENIED);return 0;
    }
    CHECK(UmiIbkrConnectionReadAccount(f->c,"DU123",10)==UMI_STATUS_OK);
    if(!strcmp(name,"wire_read_once")){
        CHECK(UmiIbkrConnectionReadAccount(f->c,"DU456",11)==UMI_STATUS_INVALID_STATE);return 0;
    }
    if(!strcmp(name,"wire_request_timeout")){
        CHECK(UmiIbkrConnectionPump(f->c,10010)==UMI_STATUS_TIMEOUT);CHECK(!f->c->snapshot.summaryComplete&&!f->c->snapshot.positionsComplete);return 0;
    }
    if(!strcmp(name,"wire_account_filter")){
        FEED(f,"63","1","35001","DU456","NetLiquidation","999999","GBP");
        CHECK(PositionFeed(f,"DU456","888","99")==0);CHECK(UmiIbkrConnectionPump(f->c,11)==UMI_STATUS_OK);
        CHECK(f->c->snapshot.valueCount==0&&f->c->snapshot.positionCount==0&&f->c->snapshot.ignoredFrames==2);return 0;
    }
    if(!strcmp(name,"wire_request_id_filter")){
        FEED(f,"63","1","999","DU123","NetLiquidation","999999","GBP");
        FEED(f,"64","1","999");CHECK(UmiIbkrConnectionPump(f->c,11)==UMI_STATUS_OK);
        CHECK(f->c->snapshot.valueCount==0&&!f->c->snapshot.summaryComplete);return 0;
    }
    if(!strcmp(name,"wire_bad_position")){
        CHECK(PositionFeed(f,"DU123","NaN","100")==0);CHECK(UmiIbkrConnectionPump(f->c,11)==UMI_STATUS_PARSE_ERROR);
        CHECK(f->c->snapshot.positionCount==0&&!f->c->snapshot.positionsComplete);return 0;
    }
    if(!strcmp(name,"wire_unterminated_fields")){
        FEED(f,"61","3","DU123","123");CHECK(UmiIbkrConnectionPump(f->c,11)==UMI_STATUS_PARSE_ERROR);return 0;
    }
    if(!strcmp(name,"wire_empty_snapshot")){
        FEED(f,"64","1","35001");FEED(f,"62","1");CHECK(UmiIbkrConnectionPump(f->c,11)==UMI_STATUS_OK);
        CHECK(f->c->snapshot.summaryComplete&&f->c->snapshot.positionsComplete&&f->c->snapshot.positionCount==0);return 0;
    }
    if(!strcmp(name,"wire_utf8_invalid")){
        FEED(f,"63","1","35001","DU123","NetLiquidation","\xc0\x80","GBP");CHECK(UmiIbkrConnectionPump(f->c,11)==UMI_STATUS_PARSE_ERROR);return 0;
    }
    if(!strcmp(name,"wire_value_capacity")){
        for(unsigned i=0;i<65U;++i){char tag[32];(void)snprintf(tag,sizeof tag,"Tag%u",i);FEED(f,"63","1","35001","DU123",tag,"1","GBP");}
        CHECK(UmiIbkrConnectionPump(f->c,11)==UMI_STATUS_OK); /* 64-frame budget */
        CHECK(UmiIbkrConnectionPump(f->c,12)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(f->c->snapshot.valueCount==64U&&!f->c->snapshot.summaryComplete);return 0;
    }
    FEED(f,"63","1","35001","DU123","NetLiquidation","12345.67","GBP");
    CHECK(PositionFeed(f,"DU123","12.125","100.01")==0);
    CHECK(UmiIbkrConnectionPump(f->c,11)==UMI_STATUS_OK);
    CHECK(!f->c->snapshot.summaryComplete&&!f->c->snapshot.positionsComplete);
    if(!strcmp(name,"wire_partial_not_complete"))return 0;
    FEED(f,"64","1","35001");FEED(f,"62","1");CHECK(UmiIbkrConnectionPump(f->c,12)==UMI_STATUS_OK);
    CHECK(f->c->snapshot.summaryComplete&&f->c->snapshot.positionsComplete);
    if(!strcmp(name,"wire_complete_snapshot")){
        CHECK(!strcmp(f->c->snapshot.values[0].value,"12345.67"));
        CHECK(!strcmp(f->c->snapshot.positions[0].quantity,"12.125"));
        CHECK(!strcmp(f->c->snapshot.positions[0].averageCost,"100.01"));return 0;
    }
    if(!strcmp(name,"wire_late_rows_ignored")){
        CHECK(PositionFeed(f,"DU123","99","99")==0);FEED(f,"63","1","35001","DU123","NetLiquidation","1","GBP");
        CHECK(UmiIbkrConnectionPump(f->c,13)==UMI_STATUS_OK);
        CHECK(!strcmp(f->c->snapshot.positions[0].quantity,"12.125")&&!strcmp(f->c->snapshot.values[0].value,"12345.67"));return 0;
    }
    return 1;
}
static int Numbers(void)
{
    uint64_t value=7;
    CHECK(UmiIbkrUnsigned("18446744073709551615",&value)&&value==UINT64_MAX);
    CHECK(!UmiIbkrUnsigned("18446744073709551616",&value)&&value==UINT64_MAX);
    CHECK(UmiIbkrDecimalText("-1234567890.123456789"));CHECK(UmiIbkrDecimalText("1.2e-12"));
    CHECK(!UmiIbkrDecimalText("nan"));CHECK(!UmiIbkrDecimalText("1,234"));CHECK(!UmiIbkrDecimalText("1e"));
    CHECK(UmiIbkrText("ملاحظات",128U,false));CHECK(!UmiIbkrText("\xed\xa0\x80",8,false));
    CHECK(!UmiIbkrText("\xf4\x90\x80\x80",8,false));CHECK(!UmiIbkrText("\033[31m",8,false));return 0;
}
static int Mutation(void)
{
    uint32_t random=7;
    for(size_t i=0;i<5000U;++i){
        Fixture *f=New();CHECK(f!=NULL);CHECK(Connect(f)==0);
        unsigned char body[48];for(size_t j=0;j<sizeof body;++j){random=random*1664525U+1013904223U;body[j]=(unsigned char)(random>>24U);}
        (void)UmiIbkrProcessFrame(f->c,body,sizeof body,10);
        CHECK(f->c->snapshot.accountCount<=UMI_IBKR_ACCOUNT_LIMIT&&f->c->snapshot.positionCount<=UMI_IBKR_POSITION_LIMIT);
        Delete(f);
    }
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *name=argv[1];
    if(!strncmp(name,"policy_",7))return Policy(name);
    if(!strncmp(name,"mapping_",8))return Mapping(name);
    if(!strcmp(name,"numbers"))return Numbers();
    if(!strcmp(name,"mutation"))return Mutation();
    Fixture *f=New();if(!f)return 2;int result=Wire(f,name);Delete(f);return result;
}
