/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_instrument_search.c
 *
 * PURPOSE:
 *   Verify typed contract discovery and bounded bond decoding using synthetic protocol frames.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
static int Bond(Fixture *f, const char *id, bool malformed)
{
    /* Synthetic bond callback in negotiated 164..176 format. */
    const char *fields[]={"18",id,"EXAMPLE","BOND","TEST-CUSIP","4.25","20301231",
        "20200101","AA","CORP","FIXED","0","0","0","Example bond","SMART","USD",
        "BONDS","CORP","321","0.01","LMT","SMART","","","0","","Example issuer",
        "","0","0","1","","1000","1000","1000"};
    return Feed(f,fields,sizeof fields/sizeof fields[0]-(malformed?1U:0U));
}
int main(int argc,char **argv)
{
    if (argc!=2) return 2;
    Fixture *f=New(); CHECK(f); CHECK(Connect(f)==0);
    UmiIbkrInstrumentQuery q={0}; strcpy(q.symbol,"EXAMPLE"); strcpy(q.securityType,"BOND");
    strcpy(q.exchange,"SMART"); strcpy(q.currency,"USD");
    uint32_t id=777U;
    if (!strcmp(argv[1],"invalid")) {
        strcpy(q.expiry,"202613");
        CHECK(UmiIbkrInstrumentSearch(f->c,&q,6U,&id)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(id==777U);
    } else if (!strcmp(argv[1],"capacity")) {
        f->c->txSize=sizeof f->c->tx;
        CHECK(UmiIbkrInstrumentSearch(f->c,&q,6U,&id)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(id==777U && !f->c->contractDetails.requestId);
    } else {
        if (!strcmp(argv[1],"futures")) {
            strcpy(q.securityType,"FUT"); strcpy(q.expiry,"203012");
        }
        CHECK(UmiIbkrInstrumentSearch(f->c,&q,6U,&id)==UMI_STATUS_OK);
        CHECK(f->c->contractDetails.instrumentLookup);
        CHECK(!strcmp(f->c->contractDetails.requestedInstrument.securityType,q.securityType));
        if (!strcmp(argv[1],"busy")) {
            uint32_t next=999U;
            CHECK(UmiIbkrInstrumentSearch(f->c,&q,7U,&next)==UMI_STATUS_BUSY);
            CHECK(next==999U && f->c->contractDetails.requestId==id);
        } else if (!strcmp(argv[1],"bond") || !strcmp(argv[1],"malformed")) {
            char text[24]; (void)snprintf(text,sizeof text,"%u",(unsigned)id);
            CHECK(Bond(f,text,!strcmp(argv[1],"malformed"))==0);
            UmiStatus pumped=UmiIbkrConnectionPump(f->c,7U);
            if (!strcmp(argv[1],"malformed")) {
                CHECK(pumped==UMI_STATUS_PARSE_ERROR && f->c->contractDetails.stale && !f->c->contractDetails.count);
            } else {
                CHECK(pumped==UMI_STATUS_OK);
                FEED(f,"52","1",text); CHECK(UmiIbkrConnectionPump(f->c,8U)==UMI_STATUS_OK);
                CHECK(f->c->contractDetails.complete && f->c->contractDetails.count==1U);
                const UmiIbkrContractDescription *r=&f->c->contractDetails.items[0];
                CHECK(r->contractId==321U && !strcmp(r->securityType,"BOND"));
                CHECK(!strcmp(r->expiry,"20301231") && !strcmp(r->minimumSize,"1000"));
                CHECK(!strcmp(r->longName,"Example issuer"));
            }
        } else if (!strcmp(argv[1],"futures")) {
            CHECK(!strcmp(f->c->contractDetails.requestedInstrument.expiry,"203012"));
        } else { Delete(f); return 2; }
    }
    Delete(f); return 0;
}
