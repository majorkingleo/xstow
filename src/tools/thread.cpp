#ifndef WIN32
#  include <unistd.h>
#  include <errno.h>
#endif


#include <stdio.h>
#include "thread.h"
#include <errno.h>
#include <string.h>
#include <cpp_util.h>

#ifdef TOOLS_USE_THREADS

#if defined(hpux) || defined(__hpux) || defined(_hpux)
#include <sys/param.h>
#include <sys/pstat.h>
#endif


#include <iostream>

namespace Tools {
                                                                       
#ifdef WIN32
DWORD __stdcall run_thread( void* ptr )
#else
extern "C" void* run_thread( void* ptr ) 
#endif
{
    Thread* thread = (Thread*) ptr;

    thread->runThread();
//    printf( "done run\n" );

#ifdef WIN32
    ExitThread(0);
#endif
    return 0;
}

#ifdef WIN32
unsigned Thread::Mutex::MutexStruct::id = 1;
#endif

Thread::Mutex::Mutex()
{
#ifdef WIN32
    char buffer[50];
    sprintf( buffer, "BS Thread Mutex %d", mutex.id++ );
    mutex.handle = CreateMutex( 0, false, buffer );
    mutex.name = buffer;
#else 
    if( pthread_mutex_init( &mutex, 0 ) != 0 )
    {
    	throw new REPORT_EXCEPTION( std::string("cannot init mutex ") + strerror(errno));
    }
#endif
}

Thread::Mutex::~Mutex()
{
#ifdef WIN32   
    CloseHandle( mutex.handle );
#else
    pthread_mutex_destroy( &mutex );
#endif
}

void Thread::Mutex::lock()
{
#ifdef WIN32
    WaitForSingleObject( mutex.handle, INFINITE);    
#else
    pthread_mutex_lock( &mutex );
#endif
}

void Thread::Mutex::unlock()
{ 
#ifdef WIN32
    ReleaseMutex( mutex.handle );
#else
    pthread_mutex_unlock( &mutex );
#endif
}

bool Thread::Mutex::locked()
{ 
#ifdef WIN32
    if( WaitForSingleObject( mutex.handle, 1L ) == WAIT_TIMEOUT )
    return true;
#else
    if( pthread_mutex_trylock( &mutex ) == EBUSY )
	return true; 
#endif
    
    unlock(); 
    return false; 
}

Thread::~Thread()
{

}

void Thread::start()
{
    int sc = getStartCount();

#ifdef WIN32
    DWORD tid; // required on WIN9x (even if M$ says only "may" here)
    thread_handle = CreateThread( NULL, 0, run_thread, this, 0, &tid );
#else
    int rv = pthread_create( &thread, 0, run_thread, this );
    if( rv != 0 ) {
         throw REPORT_EXCEPTION( format("error at pthread_create. rv: %d %s", rv, strerror(errno)) );
    }
#endif

    // wait until the thread runns
    while( !running() && getStartCount() == sc )
    {
#ifdef WIN32
    	Sleep(0);
#else    
		sched_yield();
#endif
    	if( isDone() ) {
    		break;
    	}
    }
}

int Thread::getStartCount()
{ 
    int r; 
    m_start_count.lock(); 
    r = start_count; 
    m_start_count.unlock(); 
    return r; 
}

void Thread::exit()
{
#ifdef WIN32
    ExitThread(0);
#else
    pthread_exit(0);
#endif
}

bool Thread::isDone()
{
    bool ret;
    m_done.lock();
    ret = done;
    m_done.unlock();
    return ret;
}

void Thread::runThread()
{ 
    m_running.lock(); 
    m_start_count.lock();
    start_count++; 
    m_start_count.unlock();
    run(); 
    m_start_count.lock();
    start_count--;
    m_start_count.unlock();
    m_running.unlock(); 
    m_done.lock();
    done = true;
    m_done.unlock();
}

Thread::ThreadStruct Thread::current()
{
#if defined(WIN32)
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif	
}

int Thread::processors()
{
#if defined(WIN32)
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
#else
#if defined(_SC_NPROCESSORS_ONLN)
	int result;
	if((result=sysconf(_SC_NPROCESSORS_ONLN))>0){
		return result;
	}
#elif defined(__IRIX__) && defined(_SC_NPROC_ONLN)
	int result;
	if((result=sysconf(_SC_NPROC_ONLN))>0){
		return result;
	}
#elif defined(__APPLE__)
	int result=1;
	size_t len=sizeof(result);
	if(sysctlbyname("hw.activecpu",&result,&len,NULL,0)!=-1){
		return result;
	}
#elif defined(HW_NCPU)
	int result=1;
	int mib[2]={CTL_HW,HW_NCPU};
	size_t len=sizeof(result);
	if(sysctl(mib,2,&result,&len,NULL,0)!=-1){
		return result;
	}
#elif defined(hpux) || defined(__hpux) || defined(_hpux)
	struct pst_dynamic psd;
	if(!pstat_getdynamic(&psd,sizeof(psd),(size_t)1,0)){
		return (int)psd.psd_proc_cnt;
	}
#endif
	return 1;
#endif

}

void Thread::join()
{
#ifdef WIN32
	WaitForSingleObject(thread_handle, INFINITE);
#else
	int rv = pthread_join( thread, NULL );
	if( rv != 0 ) {
		throw REPORT_EXCEPTION( format("error at pthread_create. rv: %d %s", rv, strerror(errno)) );
	}
#endif	
}

} // namespace Tools

#endif
