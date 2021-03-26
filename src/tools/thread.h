#ifndef TOOLS_THREAD_H
#define TOOLS_THREAD_H

#include "../tools_config.h"

#ifdef TOOLS_USE_THREADS

#ifdef WIN32
#  include <process.h>
#  include <windows.h>
#else
# include <pthread.h>
#endif

#include <string>

namespace Tools {

#ifdef WIN32
inline void do_sleep( unsigned s ) { Sleep( s / 1000 ); }
#else
#include <unistd.h>
inline void do_sleep( unsigned s ) { usleep( s ); }
#endif


#ifdef WIN32
DWORD __stdcall run_thread( PVOID ptr );
#else
extern "C" void* run_thread( void* ptr );
#endif


#define LOCK_RETURN( type, value, mutex ) \
  mutex.lock(); \
  type  ret = value; \
  mutex.unlock(); \
  return ret

class Thread
{
public:
    
#ifdef WIN32
    typedef int             ThreadStruct;
	HANDLE	thread_handle;
#else
    typedef pthread_t       ThreadStruct;    
#endif

    struct Mutex
    {
#ifdef WIN32           
       struct MutexStruct
       {        
            static unsigned id;
            HANDLE       handle;
            std::string    name;
       };
#else
       typedef pthread_mutex_t MutexStruct;
#endif

	MutexStruct mutex;

	Mutex();
	~Mutex();

	void lock();
	void unlock();

	bool locked();
    };	

    class MutexLock
    {
    private:
    	Mutex & mutex;

    	MutexLock( const MutexLock & other ) : mutex(other.mutex){}
    	MutexLock & operator=(  const MutexLock & other ) { return *this; }
    public:
    	MutexLock( Mutex & mutex_ )
    	: mutex( mutex_ )
    	{
    		mutex.lock();
    	}

    	~MutexLock()
    	{
    		mutex.unlock();
    	}
    };

protected:

    ThreadStruct thread;

    Mutex        m_running;
    Mutex        m_start_count;
    int          start_count;
    Mutex        m_done;
    bool         done;

public:
    Thread() { start_count = 0; done = false; }
    virtual ~Thread();

    virtual void run() = 0;

    ThreadStruct& getThread() { return thread; }

    void start();
    bool running() { return m_running.locked(); }
    void wait() { m_running.lock(); m_running.unlock(); join(); }
    void join();
    int  getStartCount();
    bool isDone();

    static ThreadStruct current();

    // return number of processors
    static int processors();

#ifdef WIN32
    friend DWORD __stdcall run_thread( PVOID ptr );
#else
    friend void* run_thread( void* ptr );
#endif

protected:
    
    void exit();

private:

    void runThread();
};

} // namespace Tools

#endif // TOOLS_USE_THREADS

#endif
