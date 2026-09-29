/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/enterprise_recovery/test_reference.c
 * PURPOSE: Compare paged results against an independent qsort/filter reference over a full dataset.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
static UmiEnterpriseRowSort sortField;
static bool descending;
static int Compare(const void *one,const void *two)
{
    const UmiEnterpriseRow *a=one,*b=two;int result;
    if(sortField==UMI_ENTERPRISE_SORT_QUANTITY)result=(a->quantity>b->quantity)-(a->quantity<b->quantity);
    else result=strcmp(sortField==UMI_ENTERPRISE_SORT_ID?a->id:a->label,sortField==UMI_ENTERPRISE_SORT_ID?b->id:b->label);
    if(result==0)return strcmp(a->id,b->id);
    result=result<0?-1:1;return descending?-result:result;
}
int main(void)
{
    TestFixture f;UmiEnterpriseRow all[64],expected[64];size_t checked=0U;
    REQUIRE(TestOpen(&f,NULL)==0);REQUIRE(TestSeed(&f)==0);
    for(unsigned group=0U;group<2U;++group){
        char csv[7000];size_t used=(size_t)snprintf(csv,sizeof(csv),"item_id,label,quantity\n");
        for(unsigned j=0U;j<32U;++j){unsigned n=group*32U+j;
            memset(&all[n],0,sizeof(all[n]));
            (void)snprintf(all[n].id,sizeof(all[n].id),"item%02u",63U-n);
            (void)snprintf(all[n].label,sizeof(all[n].label),"Group %02u",(n*17U)%11U);
            all[n].quantity=(n*37U)%23U;
            (void)snprintf(all[n].sourceJob,sizeof(all[n].sourceJob),"part%u",group);
            int bytes=snprintf(csv+used,sizeof(csv)-used,"%s,%s,%llu\n",all[n].id,all[n].label,(unsigned long long)all[n].quantity);
            REQUIRE(bytes>0&&(size_t)bytes<sizeof(csv)-used);used+=(size_t)bytes;
        }
        REQUIRE(TestApply(&f,group==0U?"part0":"part1",csv)==0);
    }
    const char *filters[]={"","Group 0","item1","part1","GROUP","not-found"};
    for(unsigned field=1U;field<=3U;++field)for(unsigned direction=0U;direction<2U;++direction)
    for(size_t filter=0U;filter<6U;++filter)for(unsigned low=0U;low<24U;low+=4U){
        UmiEnterpriseRowQuery query;UmiEnterpriseDatasetView *view=NULL;UmiEnterpriseDatasetViewInfo info;
        UmiEnterpriseRowQueryInit(&query);query.sort=(UmiEnterpriseRowSort)field;query.descending=direction!=0U;
        query.minimumQuantity=low;query.maximumQuantity=low+8U;
        (void)snprintf(query.text,sizeof(query.text),"%s",filters[filter]);
        size_t count=0U;
        for(size_t i=0U;i<64U;++i)if(all[i].quantity>=query.minimumQuantity&&all[i].quantity<=query.maximumQuantity&&
            (strstr(all[i].id,query.text)!=NULL||strstr(all[i].label,query.text)!=NULL||strstr(all[i].sourceJob,query.text)!=NULL))expected[count++]=all[i];
        sortField=query.sort;descending=query.descending;qsort(expected,count,sizeof(expected[0]),Compare);
        OK(UmiEnterpriseDatasetViewCapture(f.workspace,ACT(0),"supplies",&query,&view));
        OK(UmiEnterpriseDatasetViewDescribe(view,&info));REQUIRE(info.matchedRows==count);
        for(size_t limit=1U;limit<=16U;++limit){size_t offset=0U;do{
            UmiEnterpriseRowPage page;OK(UmiEnterpriseDatasetViewPage(view,offset,limit,&page));
            REQUIRE(page.totalRows==count&&page.offset==offset&&page.count<=limit);
            for(size_t n=0U;n<page.count;++n){REQUIRE(strcmp(page.rows[n].id,expected[offset+n].id)==0);REQUIRE(page.rows[n].quantity==expected[offset+n].quantity);}
            offset+=page.count;REQUIRE(page.nextOffset==offset&&page.complete==(offset==count));++checked;
            if(page.complete)break;
        }while(offset<count);REQUIRE(offset==count);}
        UmiEnterpriseDatasetViewDestroy(view);
    }
    printf("Independent page comparisons: %zu\n",checked);TestClose(&f);return 0;
}
