#pragma once
#include <thread> 
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <assert.h>
#define SMR_GLOBAL 0
#define SMR_LOCAL 1
#define SMR_BYTE 1
#define SMR_MB 1048576
#define SMR_GB 1073741824

class SMR
{
public:
	static void setPID(const DWORD& pid) { m_processID = pid; }
	static __int64 getGlobalMemoryValue(bool percent=true);
	static __int64 getLocalMemoryValue();
	static __int64 getPhysicalMemorySize();
	static void run(const long long ms, int flag=SMR_LOCAL, double maxLimit=0);
	static void stop();
	static bool emptySet(); // clean current process
	static bool emptyAllSet(); // clean all process
private:
	static void adjustTokenPrivilegesForNt();
	static void thread();
	static bool m_stop;
	static long long m_ms;
	static std::thread *m_thd;
	static DWORD m_processID;
	static int m_flag;
	static __int64 m_maxLimit;
};

