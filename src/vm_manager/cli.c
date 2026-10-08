/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/cli.c
 * PURPOSE:
 *   Human-facing native adapter. Commands delegate to the shared services. The interactive
 *   console accepts only named operations, never raw QMP or a shell.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Human-facing native adapter. Commands delegate to the shared services. The
 * interactive console accepts only named operations, never raw QMP or a shell.
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include "boot_internal.h"
#include <errno.h>
static void Help(void){
    puts("  qemu targets|plan|review|run (use qemu --help for standalone boots)");
    puts( "Umicom Virtual Machine Manager\n" "  --self-test | --help\n" "  runtime-pack --source DIR --inventory FILE --output NEWDIR\n" "  runtime-verify --root DIR\n" "  runtime-component --root DIR --output NEWFILE\n" "  profile-save --database FILE --id ID --name NAME --runtime DIR --image DIR\n" "       [--arch x86_64|riscv64] [--memory 1024] [--cpus 2] [--recovery 0|1]\n" "       [--disk DIR] [--expected 0]\n" "  profile-show | profile-remove --database FILE --id ID [--expected REVISION]\n" "  profile-list --database FILE\n" "  review | run --database FILE --id ID --directory NEWDIR [--expect HASH]\n" "  disk-create --runtime DIR --output NEWDIR --bytes NUMBER\n" "  disk-checkpoint --runtime DIR --disk DIR --output NEWDIR\n" "Run begins PAUSED. Its interactive commands are status, resume, pause, console,\n" "send TEXT, powerdown, quit-qemu, force-stop and leave. No TCP monitor is opened.\n" "Use a trusted qualified runtime. Hashes are not publisher signatures.");
}
typedef struct Option {
    const char*key,*value;
}
Option;
static const char *Get(const Option*o,size_t count,const char*key){
    for(size_t i=0;i<count;++i)if(!strcmp(o[i].key,key))return o[i].value;
    return NULL;
}
static int Allowed(const char*command,const char*key){
    const char*keys=!strcmp(command,"runtime-pack")?"|--source|--inventory|--output|":         !strcmp(command,"runtime-verify")?"|--root|":!strcmp(command,"runtime-component")?"|--root|--output|":         !strcmp(command,"profile-save")?"|--database|--id|--name|--runtime|--image|--disk|--arch|--memory|--cpus|--recovery|--expected|":         !strcmp(command,"profile-show")?"|--database|--id|":!strcmp(command,"profile-remove")?"|--database|--id|--expected|":         !strcmp(command,"profile-list")?"|--database|":!strcmp(command,"review")?"|--database|--id|--directory|":         !strcmp(command,"run")?"|--database|--id|--directory|--expect|":         !strcmp(command,"disk-create")?"|--runtime|--output|--bytes|":         !strcmp(command,"disk-checkpoint")?"|--runtime|--disk|--output|":"";
    char part[96];
    int n=snprintf(part,sizeof part,"|%s|",key);
    return n>0&&(size_t)n<sizeof part&&strstr(keys,part)!=NULL;
}
static int Copy(char*to,size_t cap,const char*from){
    if(!from||strlen(from)>=cap)return 0;
    strcpy(to,from);
    return 1;
}
static UmiStatus Show(const UmiVmProfile*p,void*context){
    (void)context;
    printf("%s | revision %" PRIu64 " | %s\n  %s; %u MiB; %u CPUs; %s\n  Runtime: %s\n  Image: %s\n  Disk: %s\n",p->id,p->revision,p->name,p->architecture==UMI_VM_X86_64?"x86_64":"riscv64",p->memoryMiB,p->processors,p->recovery?"recovery":"normal",p->runtimeDirectory,p->imageBundle,p->diskDirectory[0]?p->diskDirectory:"none");
    return UMI_STATUS_OK;
}
static void SafeConsole(const unsigned char*p,size_t n){
    for(size_t i=0;i<n;++i){
        unsigned char c=p[i];
        if(c=='\n'||c=='\r'||c=='\t'||(c>=32&&c<127))putchar(c);
        else printf("\\x%02x",c);
    }
    putchar('\n');
}
static int Interactive(UmiVmSession*s){
    char line[UMI_VM_CONSOLE_CHUNK];
    UmiVmReport report;
    UmiVmSnapshot state;
    int result=0;
    puts("Paused VM connected. Type resume to start guest instructions; console to read output.");
    for(;;){
        if(UmiVmObserve(s,&state)!=UMI_STATUS_OK){
            result=1;
            break;
        }
        if(!state.processRunning){
            printf("QEMU exited: %d. This is not a qualified guest-boot result.\n",state.exitCode);
            break;
        }
        fputs("umicom-vm> ",stdout);
        fflush(stdout);
        if(!fgets(line,sizeof line,stdin)){
            puts("Input closed; stopping the owned VM. Guest data may be inconsistent.");
            result=1;
            break;
        }
        size_t n=strlen(line);
        if(n&&line[n-1]=='\n')line[--n]=0;
        else if(!feof(stdin)){
            int c;
            while((c=getchar())!='\n'&&c!=EOF){
            }
            puts("Command is too long.");
            continue;
        }
        if(n&&line[n-1]=='\r')line[--n]=0;
        if(!strcmp(line,"leave")){
            puts("Use guest poweroff and wait for exit, or explicitly choose force-stop.");
            continue;
        }
        if(!strcmp(line,"force-stop")){
            (void)UmiVmForceStop(s,&report);
            puts(report.detail);
            result=1;
            break;
        }
        UmiVmCommand cmd;
        const void*input=NULL;
        size_t length=0;
        if(!strcmp(line,"status"))cmd=UMI_VM_QUERY;
        else if(!strcmp(line,"resume"))cmd=UMI_VM_RESUME;
        else if(!strcmp(line,"pause"))cmd=UMI_VM_PAUSE;
        else if(!strcmp(line,"console"))cmd=UMI_VM_CONSOLE_READ;
        else if(!strcmp(line,"powerdown"))cmd=UMI_VM_POWERDOWN;
        else if(!strcmp(line,"quit-qemu"))cmd=UMI_VM_QUIT;
        else if(!strncmp(line,"send ",5)){
            cmd=UMI_VM_CONSOLE_WRITE;
            line[n++]='\n';
            input=line+5;
            length=n-5U;
        }
        else{
            puts("Use status, resume, pause, console, send TEXT, powerdown, quit-qemu or force-stop.");
            continue;
        }
        unsigned char output[UMI_VM_CONSOLE_CHUNK];
        size_t used=0;
        UmiStatus st=UmiVmControl(s,cmd,input,length,output,sizeof output,&used,&report);
        puts(report.detail);
        if(st==UMI_STATUS_OK&&cmd==UMI_VM_CONSOLE_READ)SafeConsole(output,used);
        if(UmiVmObserve(s,&state)==UMI_STATUS_OK)printf("QMP state: %s; process: %s; control: %s\n",state.state,state.processRunning?"running":"exited",state.controlAvailable?"available":"lost");
    }
    return result;
}
/* Both packaged images and standalone boots share the same supervised console.
 * Keep Interactive as the original implementation so its commands and comments
 * remain available for review while new entry points reuse its behaviour. */
int VmSessionConsole(UmiVmSession *session)
{
    return Interactive(session);
}

int UmiVmMain(int argc,char**argv){
    /* The standalone command composes the same Framework services as umicom.
     * Existing profile and packaged-runtime commands remain unchanged. */
    if(argc>=2&&!strcmp(argv[1],"qemu"))return UmiVmBootMain(argc-2,argv+2);
    if(argc==2&&!strcmp(argv[1],"--help")){
        Help();
        return 0;
    }
    if(argc==2&&!strcmp(argv[1],"--self-test")){
        UmiVmQmpMessage m;
        const char*s="{\"return\":{\"running\":false,\"status\":\"paused\"},\"id\":1}";
        if(UmiVmQmpDecode(s,strlen(s),&m)!=UMI_STATUS_OK||m.id!=1||m.running)return 1;
        puts("Native QMP self-check passed. No database, process or network was opened.");
        return 0;
    }
    if(argc<2||(argc-2)%2){
        Help();
        return 2;
    }
    Option options[16];
    size_t count=0;
    for(int i=2;i<argc;i+=2){
        if(count==16U||!Allowed(argv[1],argv[i])||Get(options,count,argv[i])){
            fputs("Unknown or duplicate option.\n",stderr);
            return 2;
        }
        options[count++]=(Option){
            argv[i],argv[i+1]
        };
    }
#define OPT(k) Get(options,count,k)
    UmiVmReport report;
    memset(&report,0,sizeof report);
    UmiStatus status=UMI_STATUS_INVALID_ARGUMENT;
    UmiDataServer*database=NULL;
    const char*command=argv[1];
    if(!strcmp(command,"runtime-pack"))status=UmiVmRuntimePack(OPT("--source"),OPT("--inventory"),OPT("--output"),&report);
    else if(!strcmp(command,"runtime-verify"))status=UmiVmRuntimeVerify(OPT("--root"),&report);
    else if(!strcmp(command,"runtime-component"))status=UmiVmRuntimeComponent(OPT("--root"),OPT("--output"),&report);
    else if(!strcmp(command,"disk-create")){
        uint64_t bytes;
        if(OPT("--bytes")&&VmNumber(OPT("--bytes"),&bytes))status=UmiVmDiskCreate(OPT("--runtime"),OPT("--output"),bytes,&report);
    }
    else if(!strcmp(command,"disk-checkpoint"))status=UmiVmDiskCheckpoint(OPT("--runtime"),OPT("--disk"),OPT("--output"),&report);
    else if(OPT("--database")&&VmPath(OPT("--database"),0)){
        status=umi_data_server_create_sqlite(OPT("--database"),&database);
        if(status==UMI_STATUS_OK){
            UmiVmProfile profile;
            UmiVmProfileInit(&profile);
            if(!strcmp(command,"profile-list"))status=UmiVmProfileVisit(database,Show,NULL);
            else if(!strcmp(command,"profile-save")){
                uint64_t memory=1024,cpus=2,recovery=0,expected=0,revision=0;
                status=UMI_STATUS_INVALID_ARGUMENT;
                if(Copy(profile.id,sizeof profile.id,OPT("--id"))&&Copy(profile.name,sizeof profile.name,OPT("--name"))&&Copy(profile.runtimeDirectory,sizeof profile.runtimeDirectory,OPT("--runtime"))&&Copy(profile.imageBundle,sizeof profile.imageBundle,OPT("--image"))&&                     (!OPT("--disk")||Copy(profile.diskDirectory,sizeof profile.diskDirectory,OPT("--disk")))&&                     (!OPT("--memory")||VmNumber(OPT("--memory"),&memory))&&(!OPT("--cpus")||VmNumber(OPT("--cpus"),&cpus))&&(!OPT("--recovery")||VmNumber(OPT("--recovery"),&recovery))&&(!OPT("--expected")||VmNumber(OPT("--expected"),&expected))&&memory<=32768&&cpus<=16&&recovery<=1){
                    const char*arch=OPT("--arch");
                    if(!arch||!strcmp(arch,"x86_64"))profile.architecture=UMI_VM_X86_64;
                    else if(!strcmp(arch,"riscv64"))profile.architecture=UMI_VM_RISCV64;
                    else goto done;
                    profile.memoryMiB=(unsigned)memory;
                    profile.processors=(unsigned)cpus;
                    profile.recovery=(int)recovery;
                    status=UmiVmProfileSave(database,&profile,expected,&revision);
                    if(status==UMI_STATUS_OK)printf("Profile saved at revision %" PRIu64 ". No VM started.\n",revision);
                }
            }
            else if(OPT("--id")){
                status=UmiVmProfileLoad(database,OPT("--id"),&profile);
                if(status==UMI_STATUS_OK){
                    if(!strcmp(command,"profile-show"))status=Show(&profile,NULL);
                    else if(!strcmp(command,"profile-remove")){
                        uint64_t expected;
                        if(!OPT("--expected")||!VmNumber(OPT("--expected"),&expected))status=UMI_STATUS_INVALID_ARGUMENT;
                        else status=UmiVmProfileRemove(database,profile.id,expected);
                    }
                    else if(!strcmp(command,"review"))status=UmiVmReview(&profile,OPT("--directory"),&report);
                    else if(!strcmp(command,"run")){
                        UmiVmSession*session=NULL;
                        status=UmiVmStart(&profile,OPT("--directory"),OPT("--expect"),&session,&report);
                        if(status==UMI_STATUS_OK){
                            puts(report.detail);
                            int exit=Interactive(session);
                            UmiVmSessionDestroy(session);
                            umi_data_server_destroy(database);
                            return exit;
                        }
                    }
                    else status=UMI_STATUS_INVALID_ARGUMENT;
                }
            }
            else status=UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    done:     umi_data_server_destroy(database);
    if(report.detail[0])puts(report.detail);
    if(status==UMI_STATUS_OK&&report.fingerprint[0])printf("Fingerprint: %s\n",report.fingerprint);
    if(status!=UMI_STATUS_OK)fprintf(stderr,"Stopped: %s\n",UmiSetupStatusText(status));
    return status==UMI_STATUS_OK?0:status==UMI_STATUS_UNAVAILABLE?77:1;
#undef OPT
}
