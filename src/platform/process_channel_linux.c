/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Linux interactive child transport. A private socketpair supplies stdin so
 * MSG_NOSIGNAL avoids changing the application's global SIGPIPE policy.
 * stdout and stderr have distinct pipes. No interpreter is involved.
 *---------------------------------------------------------------------------*/
#define _GNU_SOURCE
#include "process_channel_internal.h"
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <time.h>
#include <unistd.h>
uint64_t PcMilliseconds(void){
    struct timespec t;
    if(clock_gettime(CLOCK_MONOTONIC,&t))return 0;
    return (uint64_t)t.tv_sec*1000U+(uint64_t)t.tv_nsec/1000000U;
}
static void Close(int *fd){
    if(*fd>=0){
        close(*fd);
        *fd=-1;
    }
}
static void Drain(UmiProcessChannel *c){
    char b[2048];
    for(unsigned i=0;i<32U&&c->error>=0;++i){
        ssize_t n=read(c->error,b,sizeof b);
        if(n>0)PcDiagnostic(c,b,(size_t)n);
        else if(n==0)Close(&c->error);
        else if(errno!=EINTR)break;
    }
}
/* Observe without reaping first: the root PID remains reserved while its
 * owned process group is drained. This avoids signalling a reused PID. */
static void Reap(UmiProcessChannel *c){
    if(!c->snapshot.running)return;
    siginfo_t info;
    memset(&info,0,sizeof info);
    if(waitid(P_PID,(id_t)c->process,&info,WEXITED|WNOHANG|WNOWAIT)||!info.si_pid)return;
    (void)kill(-c->process,SIGKILL);
    int status=0;
    pid_t n;
    do{
        n=waitpid(c->process,&status,0);
    }
    while(n<0&&errno==EINTR);
    if(n==c->process){
        c->snapshot.running=0;
        c->snapshot.exitCode=WIFEXITED(status)?WEXITSTATUS(status):128+(WIFSIGNALED(status)?WTERMSIG(status):0);
    }
}
UmiStatus UmiProcessChannelOpen(const UmiProcessChannelRequest *r,UmiProcessChannel **out){
    if(!out)return UMI_STATUS_INVALID_ARGUMENT;
    *out=NULL;
    UmiStatus s=PcValidate(r);
    if(s!=UMI_STATUS_OK)return s;
    struct stat st;
    int executable=open(r->program,O_RDONLY|O_CLOEXEC|O_NOFOLLOW);
    unsigned char header[64];
    ssize_t headerSize=executable<0?-1:pread(executable,header,sizeof header,0);
    if(executable<0||fstat(executable,&st)||!S_ISREG(st.st_mode)||!(st.st_mode&0111)||headerSize!=64||memcmp(header,"\177ELF",4)||header[4]!=2||header[5]!=1){
        if(executable>=0)close(executable);
        return UMI_STATUS_UNAVAILABLE;
    }
#if defined(__x86_64__)
    if(header[18]!=62||header[19]!=0){
        close(executable);
        return UMI_STATUS_UNAVAILABLE;
    }
#elif defined(__aarch64__)
    if(header[18]!=183||header[19]!=0){
        close(executable);
        return UMI_STATUS_UNAVAILABLE;
    }
#endif
    if(lstat(r->workingDirectory,&st)||!S_ISDIR(st.st_mode)){
        close(executable);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    char *argv[UMI_CHANNEL_MAX_ARGUMENTS+2U];
    argv[0]=(char *)r->program;
    for(size_t i=0;i<r->argumentCount;++i)argv[i+1U]=(char *)r->arguments[i];
    argv[r->argumentCount+1U]=NULL;
    /* These values are prepared before fork: only async-signal-safe calls
                             * occur in the child. QEMU's protocol-only host needs no display variables. */
    char *environment[]={
        "PATH=/usr/bin:/bin","LANG=C.UTF-8","LC_ALL=C.UTF-8",NULL
    };
    int in[2]={
        -1,-1
    },o[2]={
        -1,-1
    },e[2]={
        -1,-1
    },launch[2]={
        -1,-1
    };
    UmiProcessChannel *c=calloc(1,sizeof *c);
    if(!c){
        close(executable);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    c->input=c->output=c->error=-1;
    c->snapshot.exitCode=-1;
    if(socketpair(AF_UNIX,SOCK_STREAM|SOCK_CLOEXEC,0,in)||pipe2(o,O_CLOEXEC)||pipe2(e,O_CLOEXEC)||pipe2(launch,O_CLOEXEC)){
        s=UMI_STATUS_IO_ERROR;
        goto failed;
    }
    long maxFd=sysconf(_SC_OPEN_MAX);
    if(maxFd<0)maxFd=65536;
    pid_t parent=getpid();
    pid_t pid=fork();
    if(pid<0){
        s=UMI_STATUS_IO_ERROR;
        goto failed;
    }
    if(pid==0){
        int error=0;
        close(launch[0]);
        if(getppid()!=parent)_exit(125);
        if(prctl(PR_SET_PDEATHSIG,SIGKILL)||setpgid(0,0)||chdir(r->workingDirectory)||dup2(in[1],STDIN_FILENO)<0||dup2(o[1],STDOUT_FILENO)<0||dup2(e[1],STDERR_FILENO)<0)error=errno;
        for(int fd=3;fd<maxFd;++fd)if(fd!=launch[1]&&fd!=executable)close(fd);
        if(getppid()!=parent)_exit(125);
        if(!error)fexecve(executable,argv,environment);
        if(!error)error=errno;
        (void)write(launch[1],&error,sizeof error);
        _exit(127);
    }
    Close(&executable);
    c->process=pid;
    c->snapshot.processId=(uint64_t)pid;
    c->snapshot.running=1;
    Close(&in[1]);
    Close(&o[1]);
    Close(&e[1]);
    Close(&launch[1]);
    int error=0;
    ssize_t got;
    do{
        got=read(launch[0],&error,sizeof error);
    }
    while(got<0&&errno==EINTR);
    Close(&launch[0]);
    if(got!=0){
        s=UMI_STATUS_UNAVAILABLE;
        UmiProcessChannelTerminate(c);
        goto failed;
    }
    c->input=in[0];
    in[0]=-1;
    c->output=o[0];
    o[0]=-1;
    c->error=e[0];
    e[0]=-1;
    if(fcntl(c->input,F_SETFL,O_NONBLOCK)<0||fcntl(c->output,F_SETFL,O_NONBLOCK)<0||fcntl(c->error,F_SETFL,O_NONBLOCK)<0){
        s=UMI_STATUS_IO_ERROR;
        goto failed;
    }
    *out=c;
    return UMI_STATUS_OK;
    failed:     Close(&executable);
    Close(&in[0]);
    Close(&in[1]);
    Close(&o[0]);
    Close(&o[1]);
    Close(&e[0]);
    Close(&e[1]);
    Close(&launch[0]);
    Close(&launch[1]);
    UmiProcessChannelDestroy(c);
    return s;
}
UmiStatus UmiProcessChannelRead(UmiProcessChannel *c,void *b,size_t cap,size_t *out,unsigned timeout){
    if(!c||!b||!cap||!out||cap>65536U||timeout>60000U)return UMI_STATUS_INVALID_ARGUMENT;
    *out=0;
    uint64_t end=PcMilliseconds()+timeout;
    for(;;){
        Drain(c);
        Reap(c);
        if(c->output<0)return UMI_STATUS_OK;
        ssize_t n=read(c->output,b,cap);
        if(n>0){
            *out=(size_t)n;
            return UMI_STATUS_OK;
        }
        if(n==0){
            Close(&c->output);
            return UMI_STATUS_OK;
        }
        if(errno!=EINTR&&errno!=EAGAIN)return UMI_STATUS_IO_ERROR;
        uint64_t now=PcMilliseconds();
        if(now>=end)return UMI_STATUS_TIMEOUT;
        struct pollfd p[2]={
            {
                c->output,POLLIN,0
            },{
                c->error,POLLIN,0
            }
        };
        int remaining=(int)(end-now);
        if(remaining>25)remaining=25;
        if(poll(p,2,remaining)<0&&errno!=EINTR)return UMI_STATUS_IO_ERROR;
    }
}
UmiStatus UmiProcessChannelWrite(UmiProcessChannel *c,const void *b,size_t n,unsigned timeout){
    if(!c||(!b&&n)||n>4096U||timeout>60000U)return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t end=PcMilliseconds()+timeout;
    size_t used=0;
    while(used<n){
        Drain(c);
        Reap(c);
        if(!c->snapshot.running)return UMI_STATUS_INVALID_STATE;
        ssize_t sent=send(c->input,(const char *)b+used,n-used,MSG_NOSIGNAL);
        if(sent>0){
            used+=(size_t)sent;
            continue;
        }
        if(sent==0||(errno!=EINTR&&errno!=EAGAIN))return UMI_STATUS_IO_ERROR;
        uint64_t now=PcMilliseconds();
        if(now>=end)return UMI_STATUS_TIMEOUT;
        struct pollfd p={
            c->input,POLLOUT,0
        };
        int ms=(int)(end-now);
        if(ms>25)ms=25;
        if(poll(&p,1,ms)<0&&errno!=EINTR)return UMI_STATUS_IO_ERROR;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiProcessChannelPoll(UmiProcessChannel *c,UmiProcessChannelSnapshot *out){
    if(!c||!out)return UMI_STATUS_INVALID_ARGUMENT;
    Drain(c);
    Reap(c);
    *out=c->snapshot;
    return UMI_STATUS_OK;
}
UmiStatus UmiProcessChannelTerminate(UmiProcessChannel *c){
    if(!c)return UMI_STATUS_INVALID_ARGUMENT;
    Reap(c);
    if(c->snapshot.running){
        c->snapshot.terminated=1;
        if(kill(-c->process,SIGKILL)&&errno!=ESRCH)return UMI_STATUS_IO_ERROR;
        int st;
        while(waitpid(c->process,&st,0)<0){
            if(errno==EINTR)continue;
            break;
        }
        c->snapshot.running=0;
        c->snapshot.exitCode=137;
    }
    return UMI_STATUS_OK;
}
void UmiProcessChannelDestroy(UmiProcessChannel *c){
    if(!c)return;
    (void)UmiProcessChannelTerminate(c);
    Close(&c->input);
    Close(&c->output);
    Close(&c->error);
    free(c);
}
