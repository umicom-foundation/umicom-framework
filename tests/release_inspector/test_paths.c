/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_inspector/test_paths.c
 * PURPOSE:
 *   Check release paths and refusal of unsupported path forms.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT */
#include "test_support.h"
int TestPathsCase(const char *name)
{
    if(!strcmp(name,"paths.suffix_characters")) {
        const char *bad="<>:\"|?*\\";
        for(const char *p=bad;*p;++p) {
            char relative[128],absolute[160];
            (void)snprintf(relative,sizeof relative,"share/guide.txt%cwrong",*p);
            (void)snprintf(absolute,sizeof absolute,"C:/Umicom/guide.txt%cwrong",*p);
            CHECK(UmiSetupValidateRelative(relative)!=UMI_STATUS_OK);
            /* A backslash in an absolute Windows path is a separator, not an
             * embedded filename character. Other suffix characters are refused. */
            if(*p!='\\')CHECK(UmiSetupValidateAbsolute(absolute)!=UMI_STATUS_OK);
        }
    } else if(!strcmp(name,"paths.device_unicode")) {
        const char *bad[]={"COM\xc2\xb9","com\xc2\xb2.txt","LPT\xc2\xb3.log","CONIN$","conout$.txt","NUL.tar.gz"};
        for(size_t i=0U;i<sizeof bad/sizeof bad[0];++i)
            CHECK(UmiSetupValidateRelative(bad[i])!=UMI_STATUS_OK);
    } else if(!strcmp(name,"paths.valid_extensions")) {
        const char *good[]={"share/guide.en-GB.html",".umicom","notes.backup.1.txt","COM10.txt","company.txt","share/caf\xc3\xa9.manual.txt"};
        for(size_t i=0U;i<sizeof good/sizeof good[0];++i)OK(UmiSetupValidateRelative(good[i]));
    } else if(!strcmp(name,"catalogue.stream_refusal")) {
        const char *text="UMICOM_SUITE\t1\napp\tnotes\tUmicom Notes\tbin/notes.exe:stream\n";
        UmiSetupBundle *b=calloc(1,sizeof *b);CHECK(b);
        CHECK(ScDecode(text,strlen(text),b,0)!=UMI_STATUS_OK);UmiSetupBundleDestroy(b);
    } else return 1;
    return 0;
}
