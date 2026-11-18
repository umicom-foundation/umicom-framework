/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_entity_tag.c
 *
 * PURPOSE:
 *   Cover weak tag comparison, quoted commas and malformed conditional request tails.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/web/entity_tag.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d\n",__LINE__);return 1;}}while(0)
int main(void)
{
    bool match=false;
    const char *list=" \"other\", W/\"asset,1\" ";
    CHECK(UmiWebIfNoneMatch(list,strlen(list),"\"asset,1\"",9,&match)==UMI_STATUS_OK && match);
    CHECK(UmiWebIfNoneMatch("\"ASSET\"",7,"\"asset\"",7,&match)==UMI_STATUS_OK && !match);
    CHECK(UmiWebIfNoneMatch("*",1,NULL,0,&match)==UMI_STATUS_OK && !match);
    CHECK(UmiWebIfNoneMatch(" \t* ",4,"\"a\"",3,&match)==UMI_STATUS_OK && match);
    CHECK(UmiWebIfNoneMatch(NULL,0,"\"a\"",3,&match)==UMI_STATUS_OK && !match);
    const char *bad[]={"", "\"a\",", "\"a\",junk", "*,\"a\"", "\"a\"\r\n", "w/\"a\"", "\"a b\""};
    for(size_t i=0;i<sizeof bad/sizeof bad[0];++i) {
        match=true;
        CHECK(UmiWebIfNoneMatch(bad[i],strlen(bad[i]),"\"a\"",3,&match)==UMI_STATUS_PARSE_ERROR);
        CHECK(match);
    }
    const char nul[]={'"','a','"',0};
    CHECK(UmiWebIfNoneMatch(nul,sizeof nul,"\"a\"",3,&match)==UMI_STATUS_PARSE_ERROR && match);
    CHECK(UmiWebIfNoneMatch("\"a\"",3,"a",1,&match)==UMI_STATUS_PARSE_ERROR && match);
    return 0;
}
