/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_process/test_notes_workflow.c
 * PURPOSE: Exercise the native runner against a complete C/Assembly tool chain.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/platform/process.h"
#include "umicom/platform/clock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Line %d: %s\n",__LINE__,#x); failed=1;goto cleanup; } } while(0)

static int MakeDirectory(const char *path)
{
#ifdef _WIN32
    wchar_t wide[8192];
    return MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,wide,8192)>0 && CreateDirectoryW(wide,NULL);
#else
    return mkdir(path,0700)==0;
#endif
}
static int Join(char *out, size_t capacity, const char *root, const char *name)
{
    int written=snprintf(out,capacity,"%s/%s",root,name);
    return written>0 && (size_t)written<capacity;
}
static UmiStatus Run(const char *label,const char *program,const char *const *args,size_t count,UmiProcessResult *result)
{
    UmiProcessRequest request={0};
    request.program=program;request.arguments=args;request.argument_count=count;
    request.capture_stdout=1;request.capture_stderr=1;request.timeout_ms=90000U;
    request.window_mode=UMI_PROCESS_WINDOW_HIDDEN;
    printf("Phase: %s\n",label);fflush(stdout);
    UmiStatus status=UmiProcessExecuteWithLifetime(&request,UMI_PROCESS_LIFETIME_TREE,NULL,NULL,NULL,result);
    if(status!=UMI_STATUS_OK)fprintf(stderr,"%s: status=%d exit=%d\n%s\n",label,(int)status,result->exit_code,result->output);
    return status;
}
static int HasZip(const char *path)
{
    unsigned char header[4];
    FILE *file=fopen(path,"rb");if(file==NULL)return 0;
    size_t read=fread(header,1U,sizeof(header),file);int closed=fclose(file);
    return read==4U && closed==0 && header[0]=='P' && header[1]=='K' && header[2]==3U && header[3]==4U;
}

static int ProgramMain(int argc,char **argv)
{
    if(argc!=7)return 2;
    const char *cmake=argv[1],*ctest=argv[2],*source=argv[3],*scratch=argv[4],*compiler=argv[5],*ninja=argv[6];
    UmiProcessResult *result=calloc(1U,sizeof(*result));
    char root[8192],build[8192],install[8192],program[8192],installed[8192],note[8192],package[8192],checksum[8192];
    char compilerOption[8192],ninjaOption[8192];
    UmiClock clock=umi_clock_system();
    int failed=0,created=0;
    CHECK(result!=NULL);
    for(unsigned attempt=0U;attempt<64U && !created;++attempt){
        int written=snprintf(root,sizeof(root),"%s/notes-%llu-%u",scratch,
            (unsigned long long)clock.monotonic_nanoseconds(&clock),attempt);
        CHECK(written>0 && (size_t)written<sizeof(root));
        created=MakeDirectory(root);
    }
    CHECK(created);
    CHECK(Join(build,sizeof(build),root,"build") && Join(install,sizeof(install),root,"installed"));
    CHECK(Join(note,sizeof(note),root,"example.notes"));
#ifdef _WIN32
    CHECK(Join(program,sizeof(program),build,"umicom-notes.exe"));
    CHECK(Join(installed,sizeof(installed),install,"bin/umicom-notes.exe"));
#else
    CHECK(Join(program,sizeof(program),build,"umicom-notes"));
    CHECK(Join(installed,sizeof(installed),install,"bin/umicom-notes"));
#endif
    int written=snprintf(compilerOption,sizeof(compilerOption),"-DCMAKE_C_COMPILER=%s",compiler);
    CHECK(written>0 && (size_t)written<sizeof(compilerOption));
    written=snprintf(ninjaOption,sizeof(ninjaOption),"-DCMAKE_MAKE_PROGRAM=%s",ninja);
    CHECK(written>0 && (size_t)written<sizeof(ninjaOption));
    const char *configure[]={"-S",source,"-B",build,"-G","Ninja","-DCMAKE_BUILD_TYPE=Debug",compilerOption,ninjaOption};
    CHECK(Run("Configure C and Assembly",cmake,configure,sizeof(configure)/sizeof(configure[0]),result)==UMI_STATUS_OK);
    const char *compile[]={"--build",build,"--parallel","2"};
    CHECK(Run("Compile and link",cmake,compile,sizeof(compile)/sizeof(compile[0]),result)==UMI_STATUS_OK);
    const char *tests[]={"--test-dir",build,"--output-on-failure","--no-tests=error"};
    CHECK(Run("Execute Notes tests",ctest,tests,sizeof(tests)/sizeof(tests[0]),result)==UMI_STATUS_OK);
    const char *write[]={"--write",note,"Umicom Notes"};
    CHECK(Run("Run new note",program,write,sizeof(write)/sizeof(write[0]),result)==UMI_STATUS_OK);
    const char *inspect[]={"--inspect",note};
    CHECK(Run("Inspect C and Assembly result",program,inspect,sizeof(inspect)/sizeof(inspect[0]),result)==UMI_STATUS_OK);
    CHECK(strstr(result->output,"Bytes: 12")!=NULL && strstr(result->output,"Lines: 1")!=NULL);
    CHECK(Run("Reject overwriting an existing note",program,write,sizeof(write)/sizeof(write[0]),result)!=UMI_STATUS_OK && result->exit_code==1);
    const char *stage[]={"--install",build,"--prefix",install};
    CHECK(Run("Install to private prefix",cmake,stage,sizeof(stage)/sizeof(stage[0]),result)==UMI_STATUS_OK);
    const char *runArgument[]={note};
    CHECK(Run("Run installed application with one profile argument",installed,runArgument,
        sizeof(runArgument)/sizeof(runArgument[0]),result)==UMI_STATUS_OK);
    CHECK(strstr(result->output,"Bytes: 12")!=NULL && strstr(result->output,"Lines: 1")!=NULL);
    const char *pack[]={"--build",build,"--target","package"};
    CHECK(Run("Create ZIP package",cmake,pack,sizeof(pack)/sizeof(pack[0]),result)==UMI_STATUS_OK);
    CHECK(Join(package,sizeof(package),build,"packages/Umicom-Notes.zip") && HasZip(package));
    CHECK(Join(checksum,sizeof(checksum),build,"packages/Umicom-Notes.zip.sha256"));
    FILE *hash=fopen(checksum,"rb");CHECK(hash!=NULL);
    char line[256];char *read=fgets(line,sizeof(line),hash);int closed=fclose(hash);
    CHECK(read!=NULL && closed==0 && strlen(line)>=64U);
    for(size_t i=0U;i<64U;++i)CHECK(strchr("0123456789abcdefABCDEF",line[i])!=NULL);
    const char *hashArgs[]={"-E","sha256sum",package};
    CHECK(Run("Verify package checksum",cmake,hashArgs,sizeof(hashArgs)/sizeof(hashArgs[0]),result)==UMI_STATUS_OK && strncmp(result->output,line,64U)==0);
    printf("Completed native Notes workflow: %s\n",root);
cleanup:
    free(result);return failed;
}
#include "utf8_entry.inc"
