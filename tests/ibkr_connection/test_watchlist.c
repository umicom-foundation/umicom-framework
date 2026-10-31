/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_watchlist.c
 *
 * PURPOSE:
 *   Verify exact instrument identity, bounded storage and atomic watchlist edits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/watchlist.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while (0)
int main(int argc, char **argv)
{
    if (argc!=2) return 2;
    UmiIbkrWatchlist list={0}; strcpy(list.name,"My instruments");
    UmiIbkrWatchItem item={0}; item.contract.contractId=123U;
    strcpy(item.contract.exchange,"SMART"); strcpy(item.label,"Example share");
    CHECK(UmiIbkrWatchlistAdd(&list,&item)==UMI_STATUS_OK);
    UmiIbkrWatchlist before=list;
    if (!strcmp(argv[1],"duplicate")) {
        CHECK(UmiIbkrWatchlistAdd(&list,&item)==UMI_STATUS_ALREADY_EXISTS);
        CHECK(!memcmp(&before,&list,sizeof list));
    } else if (!strcmp(argv[1],"route")) {
        strcpy(item.contract.exchange,"NYSE");
        CHECK(UmiIbkrWatchlistAdd(&list,&item)==UMI_STATUS_OK && list.count==2U);
    } else if (!strcmp(argv[1],"invalid")) {
        item.contract.contractId=0U;
        CHECK(UmiIbkrWatchlistAdd(&list,&item)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&before,&list,sizeof list));
        item.contract.contractId=124U; strcpy(item.label,"bad\nlabel");
        CHECK(UmiIbkrWatchlistAdd(&list,&item)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&before,&list,sizeof list));
    } else if (!strcmp(argv[1],"capacity")) {
        for (uint32_t i=1U;i<UMI_IBKR_WATCHLIST_LIMIT;++i) {
            item.contract.contractId=123U+i; CHECK(UmiIbkrWatchlistAdd(&list,&item)==UMI_STATUS_OK);
        }
        before=list; item.contract.contractId=1000U;
        CHECK(UmiIbkrWatchlistAdd(&list,&item)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!memcmp(&before,&list,sizeof list));
    } else if (!strcmp(argv[1],"remove")) {
        item.contract.contractId=124U; CHECK(UmiIbkrWatchlistAdd(&list,&item)==UMI_STATUS_OK);
        CHECK(UmiIbkrWatchlistRemove(&list,0U)==UMI_STATUS_OK);
        CHECK(list.count==1U && list.items[0].contract.contractId==124U);
        CHECK(list.items[1].contract.contractId==0U);
        before=list; CHECK(UmiIbkrWatchlistRemove(&list,1U)==UMI_STATUS_NOT_FOUND);
        CHECK(!memcmp(&before,&list,sizeof list));
    } else if (!strcmp(argv[1],"unterminated")) {
        memset(item.label,'x',sizeof item.label);
        CHECK(UmiIbkrWatchlistAdd(&list,&item)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&before,&list,sizeof list));
    } else return 2;
    return 0;
}
