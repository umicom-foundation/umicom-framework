/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_export_cli.c
 *
 * PURPOSE:
 *   Exercise exclusive report options through the real shared native scan command handlers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../../tools/umicom/src/cli.h"
#include "umicom/codeguard/report.h"
#include "umicom/platform/secure_random.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define TEST_DUP _dup
#define TEST_DUP2 _dup2
#define TEST_FILENO _fileno
#define TEST_CLOSE _close
#else
#include <unistd.h>
#define TEST_DUP dup
#define TEST_DUP2 dup2
#define TEST_FILENO fileno
#define TEST_CLOSE close
#endif
#define CHECK(c) do {if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char **argv)
{
    CHECK(argc==3);
    const char *mode=argv[1],*cases[]={"quality-text","quality-json","quality-sarif","security","architecture","memory","existing","relative","missing-value","duplicate","conflict-output","conflict-summary","unknown-format","legacy-output","legacy-summary","help","stdout-new","stdout-summary","stdout-legacy","stdout-report"};
    int known=0;for(size_t i=0U;i<sizeof cases/sizeof cases[0];++i)if(strcmp(mode,cases[i])==0)known=1;
    if(!known)return 2;
    unsigned char random[16];char hex[33],leaf[160],root[UMI_PATH_CAPACITY],output[UMI_PATH_CAPACITY];
    CHECK(UmiSecureRandomBytes(random,sizeof random)==UMI_STATUS_OK);
    for(size_t i=0U;i<sizeof random;++i)(void)snprintf(hex+i*2U,3U,"%02x",(unsigned)random[i]);
    int n=snprintf(leaf,sizeof leaf,"cli-%s-%s",mode,hex);
    CHECK(n>0 && (size_t)n<sizeof leaf);
    CHECK(umi_path_join(argv[2],leaf,root,sizeof root)==UMI_STATUS_OK && !umi_fs_exists(root));
    CHECK(umi_fs_make_directories(root)==UMI_STATUS_OK);
    n=snprintf(leaf,sizeof leaf,"cli-%s-%s.json",mode,hex);
    CHECK(n>0 && (size_t)n<sizeof leaf);
    CHECK(umi_path_join(argv[2],leaf,output,sizeof output)==UMI_STATUS_OK && !umi_fs_exists(output));
    fprintf(stdout,"Retained CLI fixtures: %s ; %s\n",root,output);
    const char *format=strstr(mode,"text")!=NULL?"text":
        strstr(mode,"sarif")!=NULL?"sarif":"json";
    char *args[12]={"scan",root,"--format",(char *)format,"--output-new",output};
    int count=6,expected_exit=0,expect_file=1,sentinel=0;
    int (*command)(UmiCliContext *,int,char **)=umi_cli_command_quality;
    if(strcmp(mode,"security")==0)command=umi_cli_command_security;
    else if(strcmp(mode,"architecture")==0)command=umi_cli_command_architecture;
    else if(strcmp(mode,"memory")==0)command=umi_cli_command_memory;
    else if(strcmp(mode,"existing")==0){expected_exit=1;sentinel=1;}
    else if(strcmp(mode,"relative")==0){args[5]=leaf;expected_exit=2;expect_file=0;}
    else if(strcmp(mode,"missing-value")==0){count=5;expected_exit=2;expect_file=0;}
    else if(strcmp(mode,"duplicate")==0){args[count++]="--output-new";args[count++]=output;expected_exit=2;expect_file=0;}
    else if(strcmp(mode,"conflict-output")==0){args[count++]="--output";args[count++]=output;expected_exit=2;expect_file=0;}
    else if(strcmp(mode,"conflict-summary")==0){args[count++]="--summary";expected_exit=2;expect_file=0;}
    else if(strcmp(mode,"unknown-format")==0){args[3]="bogus";expected_exit=2;expect_file=0;}
    else if(strcmp(mode,"legacy-output")==0){args[4]="--output";sentinel=1;}
    else if(strcmp(mode,"legacy-summary")==0){args[4]="--output";args[count++]="--summary";sentinel=1;}
    else if(strcmp(mode,"help")==0){args[0]="--help";count=1;expect_file=0;}
    if(sentinel)CHECK(umi_fs_write_text(output,"original evidence")==UMI_STATUS_OK);
    if(strncmp(mode,"stdout-",7U)==0) {
        /* A real read-only descriptor rejects output at the OS boundary.
         * Restore the caller's stdout before reporting test failures. */
        char denied[UMI_PATH_CAPACITY];
        CHECK(umi_path_join(root,"readonly.txt",denied,sizeof denied)==UMI_STATUS_OK);
        CHECK(umi_fs_write_text(denied,"read-only fixture")==UMI_STATUS_OK);
        FILE *input;
#ifdef _WIN32
        wchar_t wide[UMI_PATH_CAPACITY];
        CHECK(MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,denied,-1,wide,
            (int)UMI_PATH_CAPACITY)>0);
        input=_wfopen(wide,L"rb");
#else
        input=fopen(denied,"rb");
#endif
        CHECK(input!=NULL);
        CHECK(fflush(stdout)==0);
        int saved=TEST_DUP(TEST_FILENO(stdout));CHECK(saved>=0);
        CHECK(TEST_DUP2(TEST_FILENO(input),TEST_FILENO(stdout))>=0);
        if(strcmp(mode,"stdout-summary")==0){args[4]="--summary";count=5;expect_file=0;}
        else if(strcmp(mode,"stdout-legacy")==0)args[4]="--output";
        else if(strcmp(mode,"stdout-report")==0){count=4;expect_file=0;}
        expected_exit=1;
        int actual=command(NULL,count,args);
        (void)fflush(stdout);
        int restored=TEST_DUP2(saved,TEST_FILENO(stdout));
        (void)TEST_CLOSE(saved);
        (void)fclose(input);
        clearerr(stdout);
        CHECK(restored>=0 && actual==expected_exit);
    }else CHECK(command(NULL,count,args)==expected_exit);
    CHECK(umi_fs_exists(output)==expect_file);
    if(expect_file){
        char *contents=NULL;size_t size=0U;
        CHECK(umi_fs_read_text(output,&contents,&size)==UMI_STATUS_OK);
        if(strcmp(mode,"existing")==0 || strcmp(mode,"legacy-summary")==0)
            CHECK(size==17U && strcmp(contents,"original evidence")==0);
        else{
            UmiCodeGuardResult *empty=NULL;
            CHECK(umi_codeguard_result_create(1U,&empty)==UMI_STATUS_OK);
            UmiCodeGuardReportFormat selected=strcmp(format,"text")==0?UMI_CODEGUARD_REPORT_TEXT:
                strcmp(format,"sarif")==0?UMI_CODEGUARD_REPORT_SARIF:UMI_CODEGUARD_REPORT_JSON;
            char *expected=NULL;size_t expected_size=0U;
            CHECK(umi_codeguard_report_render(selected,empty,SIZE_MAX-1U,&expected,&expected_size)==UMI_STATUS_OK);
            CHECK(size==expected_size && memcmp(contents,expected,size)==0);
            umi_codeguard_report_free(expected);umi_codeguard_result_destroy(empty);
        }
        umi_fs_free_text(contents);
    }
    return 0;
}
