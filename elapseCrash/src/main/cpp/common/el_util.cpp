//
// Created by 吴东生 on 2023/11/16.
//

#include "el_util.h"


#define PROCESS_NAME_LENGTH 512
#define THREAD_NAME_LENGTH 512

char* getClassName(JNIEnv *env,  jclass anyClass)
{
    // Get the JNIEnv class
    jclass classClass = env->FindClass( EL_CLASS_NAME);
    // Get the getName method ID
    jmethodID getNameMethod = env->GetMethodID(classClass, EL_GET_NAME_METHOD_NAME, EL_CET_NAME_METHOD_SIGNATURE);
    // Call the getName method to get the class name
    auto className = (jstring)env->CallObjectMethod( anyClass, getNameMethod);
    // Convert the Java string to a C string
    const char *classNameStr = env->GetStringUTFChars( className, nullptr);
    // Print or use the class name as needed
    printf("Class name: %s\n", classNameStr);
    // Release the C string
    env->ReleaseStringUTFChars( className, classNameStr);

    return const_cast<char *>(classNameStr);
}


const char* xcc_util_get_signame(const siginfo_t* si)
{
    switch (si->si_signo)
    {
        case SIGABRT:   return "SIGABRT";
        case SIGBUS:    return "SIGBUS";
        case SIGFPE:    return "SIGFPE";
        case SIGILL:    return "SIGILL";
        case SIGSEGV:   return "SIGSEGV";
        case SIGTRAP:   return "SIGTRAP";
        case SIGSYS:    return "SIGSYS";
        case SIGSTKFLT: return "SIGSTKFLT";
        default:        return "?";
    }
}

const char* xcc_util_get_sigcodename(const siginfo_t* si)
{
    // Try the signal-specific codes...
    switch (si->si_signo) {
        case SIGBUS:
            switch(si->si_code)
            {
                case BUS_ADRALN:    return "BUS_ADRALN";
                case BUS_ADRERR:    return "BUS_ADRERR";
                case BUS_OBJERR:    return "BUS_OBJERR";
                case BUS_MCEERR_AR: return "BUS_MCEERR_AR";
                case BUS_MCEERR_AO: return "BUS_MCEERR_AO";
                default:            break;
            }
            break;
        case SIGFPE:
            switch(si->si_code)
            {
                case FPE_INTDIV:   return "FPE_INTDIV";
                case FPE_INTOVF:   return "FPE_INTOVF";
                case FPE_FLTDIV:   return "FPE_FLTDIV";
                case FPE_FLTOVF:   return "FPE_FLTOVF";
                case FPE_FLTUND:   return "FPE_FLTUND";
                case FPE_FLTRES:   return "FPE_FLTRES";
                case FPE_FLTINV:   return "FPE_FLTINV";
                case FPE_FLTSUB:   return "FPE_FLTSUB";
                default:           break;
            }
            break;
        case SIGILL:
            switch(si->si_code)
            {
                case ILL_ILLOPC:   return "ILL_ILLOPC";
                case ILL_ILLOPN:   return "ILL_ILLOPN";
                case ILL_ILLADR:   return "ILL_ILLADR";
                case ILL_ILLTRP:   return "ILL_ILLTRP";
                case ILL_PRVOPC:   return "ILL_PRVOPC";
                case ILL_PRVREG:   return "ILL_PRVREG";
                case ILL_COPROC:   return "ILL_COPROC";
                case ILL_BADSTK:   return "ILL_BADSTK";
                default:           break;
            }
            break;
        case SIGSEGV:
            switch(si->si_code)
            {
                case SEGV_MAPERR:  return "SEGV_MAPERR";
                case SEGV_ACCERR:  return "SEGV_ACCERR";
                case SEGV_BNDERR:  return "SEGV_BNDERR";
                case SEGV_PKUERR:  return "SEGV_PKUERR";
                default:           break;
            }
            break;
        case SIGTRAP:
            switch(si->si_code)
            {
                case TRAP_BRKPT:   return "TRAP_BRKPT";
                case TRAP_TRACE:   return "TRAP_TRACE";
                case TRAP_BRANCH:  return "TRAP_BRANCH";
                case TRAP_HWBKPT:  return "TRAP_HWBKPT";
                default:           break;
            }
            if((si->si_code & 0xff) == SIGTRAP)
            {
                switch((si->si_code >> 8) & 0xff)
                {
                    case PTRACE_EVENT_FORK:       return "PTRACE_EVENT_FORK";
                    case PTRACE_EVENT_VFORK:      return "PTRACE_EVENT_VFORK";
                    case PTRACE_EVENT_CLONE:      return "PTRACE_EVENT_CLONE";
                    case PTRACE_EVENT_EXEC:       return "PTRACE_EVENT_EXEC";
                    case PTRACE_EVENT_VFORK_DONE: return "PTRACE_EVENT_VFORK_DONE";
                    case PTRACE_EVENT_EXIT:       return "PTRACE_EVENT_EXIT";
                    case PTRACE_EVENT_SECCOMP:    return "PTRACE_EVENT_SECCOMP";
                    case PTRACE_EVENT_STOP:       return "PTRACE_EVENT_STOP";
                    default:                      break;
                }
            }
            break;
        case SIGSYS:
            switch(si->si_code)
            {
                case SYS_SECCOMP: return "SYS_SECCOMP";
                default:          break;
            }
            break;
        default:
            break;
    }

    // Then the other codes...
    switch (si->si_code) {
        case SI_USER:     return "SI_USER";
        case SI_KERNEL:   return "SI_KERNEL";
        case SI_QUEUE:    return "SI_QUEUE";
        case SI_TIMER:    return "SI_TIMER";
        case SI_MESGQ:    return "SI_MESGQ";
        case SI_ASYNCIO:  return "SI_ASYNCIO";
        case SI_SIGIO:    return "SI_SIGIO";
        case SI_TKILL:    return "SI_TKILL";
        case SI_DETHREAD: return "SI_DETHREAD";
    }

    // Then give up...
    return "?";
}

int xcc_util_signal_has_si_addr(const siginfo_t* si)
{
    //manually sent signals won't have si_addr
    if(si->si_code == SI_USER || si->si_code == SI_QUEUE || si->si_code == SI_TKILL) return 0;

    switch (si->si_signo)
    {
        case SIGBUS:
        case SIGFPE:
        case SIGILL:
        case SIGSEGV:
        case SIGTRAP:
            return 1;
        default:
            return 0;
    }
}

int xcc_util_signal_has_sender(const siginfo_t* si, pid_t caller_pid)
{
    return (SI_FROMUSER(si) && (si->si_pid != 0) && (si->si_pid != caller_pid)) ? 1 : 0;
}


const char* getProcessName(pid_t pid){
    //读文件
    if (pid<=1)return nullptr;
    char *line=(char*)calloc(1,PROCESS_NAME_LENGTH);
    char *path= (char*)calloc(1,PATH_MAX);
    snprintf(path,PATH_MAX,"/proc/%d/cmdline",pid);
    FILE *cmdFile=nullptr;
    if(cmdFile= fopen(path,"r")){
        fgets(line,PROCESS_NAME_LENGTH,cmdFile);
        fclose(cmdFile);
    }
    if (line){
        int length= strlen(line);
        if(line[length-1]=='\n'){
            line[length-1]='\0';
        }
    }
    free(path);
    return line;

}


const char*  getThreadName( pid_t pid){
    //读文件
    if (pid<=1)return nullptr;
    char *line=(char*)calloc(1,THREAD_NAME_LENGTH);
    char *path= (char*)calloc(1,PATH_MAX);
    snprintf(path,PATH_MAX,"/proc/%d/comm",pid);
    FILE *cmdFile=nullptr;
    if(cmdFile= fopen(path,"r")){
        fgets(line,THREAD_NAME_LENGTH,cmdFile);
        fclose(cmdFile);
    }
    if (line){
        int length= strlen(line);
        if(line[length-1]=='\n'){
            line[length-1]='\0';
        }
    }
    free(path);
    return line;

}