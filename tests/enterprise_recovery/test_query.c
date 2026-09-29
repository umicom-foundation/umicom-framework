/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/enterprise_recovery/test_query.c
 * PURPOSE: Check deterministic ordering, filtering, exact page boundaries and owned lifetimes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int TestQueries(const char *name)
{
    TestFixture f;UmiEnterpriseDatasetView *view=NULL;
    UmiEnterpriseRowQuery query;UmiEnterpriseRowPage page;
    UmiEnterpriseDatasetViewInfo info;
    REQUIRE(TestOpen(&f,NULL)==0);REQUIRE(TestSeed(&f)==0);
    const char csv[]="item_id,label,quantity\nb,Zebra,8\na,Caf\xc3\xa9 notebooks,20\nc,Alpha,20\nd,\"Two\nlines\",0\n";
    if(strcmp(name,"query_empty")!=0)REQUIRE(TestApply(&f,"opening",csv)==0);
    UmiEnterpriseRowQueryInit(&query);
    uint64_t before=TestRevision(&f);
    if(strcmp(name,"query_defaults")==0){REQUIRE(query.sort==UMI_ENTERPRISE_SORT_ID&&query.maximumQuantity==(uint64_t)INT64_MAX&&!query.descending);}
    else if(strcmp(name,"query_sort_id_desc")==0)query.descending=true;
    else if(strcmp(name,"query_sort_label")==0)query.sort=UMI_ENTERPRISE_SORT_LABEL;
    else if(strcmp(name,"query_sort_label_desc")==0){query.sort=UMI_ENTERPRISE_SORT_LABEL;query.descending=true;}
    else if(strcmp(name,"query_sort_quantity")==0)query.sort=UMI_ENTERPRISE_SORT_QUANTITY;
    else if(strcmp(name,"query_sort_quantity_desc")==0){query.sort=UMI_ENTERPRISE_SORT_QUANTITY;query.descending=true;}
    else if(strcmp(name,"query_filter_unicode")==0)(void)snprintf(query.text,sizeof(query.text),"\xc3\xa9 note");
    else if(strcmp(name,"query_filter_origin")==0)(void)snprintf(query.text,sizeof(query.text),"opening");
    else if(strcmp(name,"query_filter_multiline")==0)(void)snprintf(query.text,sizeof(query.text),"Two\nlines");
    else if(strcmp(name,"query_filter_case")==0)(void)snprintf(query.text,sizeof(query.text),"ZEBRA");
    else if(strcmp(name,"query_filter_none")==0)(void)snprintf(query.text,sizeof(query.text),"not-present");
    else if(strcmp(name,"query_range")==0){query.minimumQuantity=8U;query.maximumQuantity=20U;}
    else if(strcmp(name,"query_exact_quantity")==0){query.minimumQuantity=20U;query.maximumQuantity=20U;}
    else if(strcmp(name,"query_invalid_utf8")==0){query.text[0]=(char)0xc0;query.text[1]=(char)0xaf;}
    else if(strcmp(name,"query_unterminated")==0)memset(query.text,'x',sizeof(query.text));
    else if(strcmp(name,"query_invalid_sort")==0)query.sort=(UmiEnterpriseRowSort)99;
    else if(strcmp(name,"query_reverse_range")==0){query.minimumQuantity=9U;query.maximumQuantity=8U;}
    else if(strcmp(name,"query_range_overflow")==0)query.maximumQuantity=UINT64_MAX;
    else if(strcmp(name,"query_64_rows")==0){
        char batch[3000];size_t used;
        for(unsigned group=0U;group<2U;++group){used=(size_t)snprintf(batch,sizeof(batch),"item_id,label,quantity\n");
            for(unsigned n=0U;n<32U;++n){int bytes=snprintf(batch+used,sizeof(batch)-used,"item%02u,Item %02u,%u\n",group*32U+n,group*32U+n,n);REQUIRE(bytes>0&&(size_t)bytes<sizeof(batch)-used);used+=(size_t)bytes;}
            if(group==0U){TestClose(&f);REQUIRE(TestOpen(&f,NULL)==0);REQUIRE(TestSeed(&f)==0);}
            REQUIRE(TestApply(&f,group==0U?"first":"second",batch)==0);
        }
        before=TestRevision(&f);
    }
    const char *dataset=strcmp(name,"query_missing")==0?"missing":"supplies";
    UmiEnterpriseActor actor=strcmp(name,"query_denied")==0?(UmiEnterpriseActor){"intruder",NULL}:ACT(0);
    UmiStatus status=UmiEnterpriseDatasetViewCapture(f.workspace,actor,dataset,&query,&view);
    if(strstr(name,"invalid")!=NULL||strcmp(name,"query_unterminated")==0||strcmp(name,"query_reverse_range")==0||strcmp(name,"query_range_overflow")==0){REQUIRE(status==UMI_STATUS_INVALID_ARGUMENT&&view==NULL);}
    else if(strcmp(name,"query_denied")==0){REQUIRE(status==UMI_STATUS_PERMISSION_DENIED&&view==NULL);}
    else if(strcmp(name,"query_missing")==0){REQUIRE(status==UMI_STATUS_NOT_FOUND&&view==NULL);}
    else {
        REQUIRE(status==UMI_STATUS_OK);OK(UmiEnterpriseDatasetViewDescribe(view,&info));OK(UmiEnterpriseDatasetViewPage(view,0U,16U,&page));
        REQUIRE(info.workspaceRevision==before&&TestRevision(&f)==before);
        if(strcmp(name,"query_empty")==0||strcmp(name,"query_filter_case")==0||strcmp(name,"query_filter_none")==0)REQUIRE(page.count==0U&&page.complete);
        else if(strcmp(name,"query_sort_id_desc")==0)REQUIRE(strcmp(page.rows[0].id,"d")==0);
        else if(strcmp(name,"query_sort_label")==0)REQUIRE(strcmp(page.rows[0].id,"c")==0);
        else if(strcmp(name,"query_sort_label_desc")==0)REQUIRE(strcmp(page.rows[0].id,"b")==0);
        else if(strcmp(name,"query_sort_quantity")==0)REQUIRE(strcmp(page.rows[0].id,"d")==0&&strcmp(page.rows[2].id,"a")==0&&strcmp(page.rows[3].id,"c")==0);
        else if(strcmp(name,"query_sort_quantity_desc")==0)REQUIRE(strcmp(page.rows[0].id,"a")==0&&strcmp(page.rows[1].id,"c")==0);
        else if(strcmp(name,"query_filter_unicode")==0)REQUIRE(page.count==1U&&strcmp(page.rows[0].id,"a")==0);
        else if(strcmp(name,"query_filter_multiline")==0)REQUIRE(page.count==1U&&strcmp(page.rows[0].id,"d")==0);
        else if(strcmp(name,"query_range")==0)REQUIRE(page.count==3U);
        else if(strcmp(name,"query_exact_quantity")==0)REQUIRE(page.count==2U);
        else if(strcmp(name,"query_page_edges")==0){
            OK(UmiEnterpriseDatasetViewPage(view,4U,1U,&page));REQUIRE(page.count==0U&&page.complete&&page.nextOffset==4U);
            UmiEnterpriseRowPage sentinel;memset(&sentinel,0x5a,sizeof(sentinel));page=sentinel;
            STATUS(UmiEnterpriseDatasetViewPage(view,5U,1U,&page),UMI_STATUS_NOT_FOUND);REQUIRE(memcmp(&sentinel,&page,sizeof(page))==0);
            STATUS(UmiEnterpriseDatasetViewPage(view,SIZE_MAX,1U,&page),UMI_STATUS_NOT_FOUND);
            STATUS(UmiEnterpriseDatasetViewPage(view,0U,0U,&page),UMI_STATUS_INVALID_ARGUMENT);
            STATUS(UmiEnterpriseDatasetViewPage(view,0U,17U,&page),UMI_STATUS_INVALID_ARGUMENT);
        }else if(strcmp(name,"query_64_rows")==0){
            REQUIRE(page.count==16U&&!page.complete&&page.totalRows==64U);
            for(size_t i=0U;i<64U;++i){OK(UmiEnterpriseDatasetViewPage(view,i,1U,&page));char id[20];(void)snprintf(id,sizeof(id),"item%02zu",i);REQUIRE(strcmp(page.rows[0].id,id)==0);}
        }else if(strcmp(name,"query_frozen")==0){
            REQUIRE(TestApply(&f,"changed","item_id,label,quantity\na,Changed,33\n")==0);
            OK(UmiEnterpriseDatasetViewPage(view,0U,16U,&page));REQUIRE(page.rows[0].quantity==20U&&page.workspaceRevision==before);
        }else if(strcmp(name,"query_lifetime")==0){TestClose(&f);OK(UmiEnterpriseDatasetViewPage(view,0U,16U,&page));REQUIRE(page.count==4U);}
        else if(strcmp(name,"query_copy_isolation")==0){page.rows[0].quantity=900U;OK(UmiEnterpriseDatasetViewPage(view,0U,16U,&page));REQUIRE(page.rows[0].quantity==20U);}
        else REQUIRE(page.count==4U);
    }
    UmiEnterpriseDatasetViewDestroy(view);TestClose(&f);return 0;
}
