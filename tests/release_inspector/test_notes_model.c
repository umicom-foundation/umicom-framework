/* Umicom Foundation | Sammy Hegab | MIT
 * This portable model test is not the Windows installed-startup test. */
#include "../../examples/release_notes/model.h"
#include <stdio.h>
#include <string.h>
#define REQUIRE(test) do { if(!(test)){fprintf(stderr,"line %d: %s\n",__LINE__,#test);return 1;} } while(0)
int main(void)
{
    uint32_t words=999;
    REQUIRE(!UmiReleaseNotesWordCount("",0,&words) && words==0);
    REQUIRE(!UmiReleaseNotesWordCount("one two\r\nthree\tfour",strlen("one two\r\nthree\tfour"),&words) && words==4);
    REQUIRE(!UmiReleaseNotesWordCount(" caf\xc3\xa9 ",7,&words) && words==1);
    REQUIRE(UmiReleaseNotesWordCount(NULL,1,&words) && words==0);
    REQUIRE(UmiReleaseNotesWordCount("one",3,NULL));
    REQUIRE(UmiReleaseNotesWordCount("a\0b",3,&words) && words==0);
    char text[32769];memset(text,'a',sizeof text);
    REQUIRE(!UmiReleaseNotesWordCount(text,32768,&words) && words==1);
    REQUIRE(UmiReleaseNotesWordCount(text,32769,&words) && words==0);
    puts("Notes release-laboratory model checks passed. No Windows program was run.");
    return 0;
}
