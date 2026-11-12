/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_cli_compiler_plan.c
 *
 * PURPOSE:
 *   Verify native compiler plan option admission without executing any compiler.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "support/assertions.h"
#include <assert.h>
#include <string.h>
#include "cli.h"

static char *base[] = {"plan","--provider","gcc","--executable","not-a-real-driver",
    "--language","c","--action","compile","--standard","c17","--source","not-a-real-source.c",
    "--working-directory","not-a-real-directory","--output","not-created.o"};
static char *args[1024];

static int Reset(void)
{
    (void)memcpy(args,base,sizeof(base));
    return (int)(sizeof(base)/sizeof(base[0]));
}

/* Admission coverage now includes explicit PIC intent, duplicate switches and unsupported combinations. The previous implementation is retained for review. */
#if 0
int main(void)
{
    char *help[] = {"--help"}, *nested[] = {"plan","--help"};
    assert(umi_cli_command_compiler(1,help) == 0);
    assert(umi_cli_command_compiler(2,nested) == 0);
    assert(umi_cli_command_compiler(0,NULL) == 2);
    assert(umi_cli_command_compiler(-1,NULL) == 2);
    assert(umi_cli_command_compiler(1025,NULL) == 2);
    assert(umi_cli_command_compiler(1,NULL) == 2);
    int count = Reset();
    assert(umi_cli_command_compiler(count,args) == 0);
    /* Remove each required pair, and separately refuse duplicate singletons. */
    for (int index = 1; index < count; index += 2) {
        (void)Reset();
        (void)memmove(args + index,args + index + 2,(size_t)(count-index-2)*sizeof(args[0]));
        assert(umi_cli_command_compiler(count-2,args) == 2);
        (void)Reset();
        args[count] = base[index]; args[count+1] = base[index+1];
        assert(umi_cli_command_compiler(count+2,args) == 2);
    }
    (void)Reset(); args[4] = NULL;
    assert(umi_cli_command_compiler(count,args) == 2);
    (void)Reset(); args[4] = "";
    assert(umi_cli_command_compiler(count,args) == 2);
    (void)Reset(); args[4] = "--language";
    assert(umi_cli_command_compiler(count,args) == 2);
    (void)Reset(); args[0] = "run";
    assert(umi_cli_command_compiler(count,args) == 2);
    (void)Reset(); args[2] = "rust";
    assert(umi_cli_command_compiler(count,args) == 2);
    (void)Reset(); args[6] = "rust";
    assert(umi_cli_command_compiler(count,args) == 2);
    (void)Reset(); args[8] = "link";
    assert(umi_cli_command_compiler(count,args) == 2);
    (void)Reset(); args[count] = "--apply";
    assert(umi_cli_command_compiler(count+1,args) == 2);
    (void)Reset(); args[count] = "--option"; args[count+1] = "-O2";
    assert(umi_cli_command_compiler(count+2,args) == 2);
    (void)Reset(); args[8] = "check";
    assert(umi_cli_command_compiler(count,args) == 2); /* output is not used by CHECK */
    assert(umi_cli_command_compiler(count-2,args) == 0);
    args[count-2] = "--debug";
    assert(umi_cli_command_compiler(count-1,args) == 2);
    char *switches[] = {"--debug","--warnings-as-errors"};
    for (size_t i = 0U; i < sizeof(switches)/sizeof(switches[0]); ++i) {
        (void)Reset(); args[count] = switches[i];
        args[count+1] = switches[i];
        assert(umi_cli_command_compiler(count+2,args) == 2);
    }
    char long_text[2049];
    (void)memset(long_text,'x',sizeof(long_text)-1U); long_text[sizeof(long_text)-1U] = '\0';
    const int text_slots[] = {4,10,12,14,16};
    for (size_t i = 0U; i < sizeof(text_slots)/sizeof(text_slots[0]); ++i) {
        (void)Reset(); args[text_slots[i]] = long_text;
        assert(umi_cli_command_compiler(count,args) == 2);
    }
    char *repeated[] = {"--include","--define"};
    for (size_t i = 0U; i < sizeof(repeated)/sizeof(repeated[0]); ++i) {
        (void)Reset(); int total = count;
        for (size_t row = 0U; row < 33U; ++row) {
            args[total++] = repeated[i]; args[total++] = "value";
        }
        assert(umi_cli_command_compiler(total,args) == 2);
        assert(umi_cli_command_compiler(total-2,args) == 0);
    }
    return 0;
}
#endif
int main(void)
{
    char *help[] = {"--help"}, *nested[] = {"plan","--help"};
    assert(umi_cli_command_compiler(1,help) == 0);
    assert(umi_cli_command_compiler(2,nested) == 0);
    assert(umi_cli_command_compiler(0,NULL) == 2);
    assert(umi_cli_command_compiler(-1,NULL) == 2);
    assert(umi_cli_command_compiler(1025,NULL) == 2);
    assert(umi_cli_command_compiler(1,NULL) == 2);
    int count = Reset();
    assert(umi_cli_command_compiler(count,args) == 0);
    /* Remove each required pair, and separately refuse duplicate singletons. */
    for (int index = 1; index < count; index += 2) {
        (void)Reset();
        (void)memmove(args + index,args + index + 2,(size_t)(count-index-2)*sizeof(args[0]));
        assert(umi_cli_command_compiler(count-2,args) == 2);
        (void)Reset();
        args[count] = base[index]; args[count+1] = base[index+1];
        assert(umi_cli_command_compiler(count+2,args) == 2);
    }
    (void)Reset(); args[4] = NULL;
    assert(umi_cli_command_compiler(count,args) == 2);
    (void)Reset(); args[4] = "";
    assert(umi_cli_command_compiler(count,args) == 2);
    (void)Reset(); args[4] = "--language";
    assert(umi_cli_command_compiler(count,args) == 2);
    (void)Reset(); args[0] = "run";
    assert(umi_cli_command_compiler(count,args) == 2);
    (void)Reset(); args[2] = "rust";
    assert(umi_cli_command_compiler(count,args) == 2);
    (void)Reset(); args[6] = "rust";
    assert(umi_cli_command_compiler(count,args) == 2);
    (void)Reset(); args[8] = "link";
    assert(umi_cli_command_compiler(count,args) == 2);
    (void)Reset(); args[count] = "--apply";
    assert(umi_cli_command_compiler(count+1,args) == 2);
    (void)Reset(); args[count] = "--option"; args[count+1] = "-O2";
    assert(umi_cli_command_compiler(count+2,args) == 2);
    (void)Reset(); args[8] = "check";
    assert(umi_cli_command_compiler(count,args) == 2); /* output is not used by CHECK */
    assert(umi_cli_command_compiler(count-2,args) == 0);
    args[count-2] = "--debug";
    assert(umi_cli_command_compiler(count-1,args) == 2);
    char *switches[] = {"--debug","--warnings-as-errors","--position-independent"};
    for (size_t i = 0U; i < sizeof(switches)/sizeof(switches[0]); ++i) {
        (void)Reset(); args[count] = switches[i];
        args[count+1] = switches[i];
        assert(umi_cli_command_compiler(count+2,args) == 2);
    }
    (void)Reset(); args[count] = "--position-independent";
    assert(umi_cli_command_compiler(count+1,args) == 0);
    args[2] = "clang";
    assert(umi_cli_command_compiler(count+1,args) == 0);
    args[2] = "msvc";
    assert(umi_cli_command_compiler(count+1,args) == 2);
    (void)Reset(); args[8] = "check"; args[count-2] = "--position-independent";
    assert(umi_cli_command_compiler(count-1,args) == 2);
    char long_text[2049];
    (void)memset(long_text,'x',sizeof(long_text)-1U); long_text[sizeof(long_text)-1U] = '\0';
    const int text_slots[] = {4,10,12,14,16};
    for (size_t i = 0U; i < sizeof(text_slots)/sizeof(text_slots[0]); ++i) {
        (void)Reset(); args[text_slots[i]] = long_text;
        assert(umi_cli_command_compiler(count,args) == 2);
    }
    char *repeated[] = {"--include","--define"};
    for (size_t i = 0U; i < sizeof(repeated)/sizeof(repeated[0]); ++i) {
        (void)Reset(); int total = count;
        for (size_t row = 0U; row < 33U; ++row) {
            args[total++] = repeated[i]; args[total++] = "value";
        }
        assert(umi_cli_command_compiler(total,args) == 2);
        assert(umi_cli_command_compiler(total-2,args) == 0);
    }
    return 0;
}
