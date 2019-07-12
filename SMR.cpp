#include "SMR.h"

bool SMR::m_stop = false;
long long SMR::m_ms = 10000;
std::thread *SMR::m_thd = nullptr;
DWORD SMR::m_processID = GetCurrentProcessId();
int SMR::m_flag = SMR_LOCAL;
__int64 SMR::m_maxLimit = 0;

void SMR::run(const long long ms, int flag, double maxLimit)
{
	m_flag = flag;
	if (maxLimit > 0. && maxLimit < 1.)
	{
		DWORDLONG phyM = getPhysicalMemorySize();
		m_maxLimit = (phyM * maxLimit);
	}
	else m_maxLimit = maxLimit;

	m_ms = ms;
	m_stop = false;
	adjustTokenPrivilegesForNt();
	m_thd = new std::thread(thread);
}

void SMR::stop()
{
	m_stop = true;
	m_thd->join();
}

void SMR::thread()
{
	while (!m_stop)
	{
		if ((m_flag == SMR_LOCAL && (m_maxLimit == 0 || getLocalMemoryValue() > m_maxLimit ) && !emptySet()) || 
			(m_flag == SMR_GLOBAL && (m_maxLimit == 0 || getGlobalMemoryValue() > m_maxLimit) && !emptyAllSet()))
		{
			printf("Can't clean the memory.\n");
			return;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(m_ms));
	}
}

bool SMR::emptyAllSet()
{
	HANDLE SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (SnapShot == NULL)
	{
		printf("Open Process failed.\n");
		return FALSE;
	}
	PROCESSENTRY32 ProcessInfo;
	ProcessInfo.dwSize = sizeof(ProcessInfo);
	int count = 0;
	BOOL Status = Process32First(SnapShot, &ProcessInfo);
	while (Status)
	{
		count++;
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE,
			ProcessInfo.th32ProcessID);
		if (hProcess)
		{
			SetProcessWorkingSetSize(hProcess, -1, -1);
			::EmptyWorkingSet(hProcess);
			CloseHandle(hProcess);
		}
		Status = Process32Next(SnapShot, &ProcessInfo);
	}
	return TRUE;
}

bool SMR::emptySet()
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, m_processID);
	if (!hProcess)
	{
		printf("Open Process failed.\n");
		return false;
	}

	SetProcessWorkingSetSize(hProcess, -1, -1);
	::EmptyWorkingSet(hProcess);
	CloseHandle(hProcess);
	return true;
}

void SMR::adjustTokenPrivilegesForNt()
{
	HANDLE hHandle;
	TOKEN_PRIVILEGES tkp;
	::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hHandle);
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	::AdjustTokenPrivileges(hHandle, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
}

__int64 SMR::getPhysicalMemorySize()
{
	MEMORYSTATUSEX stat;
	stat.dwLength = sizeof(stat);
	if (::GlobalMemoryStatusEx(&stat))
	{
		return stat.ullTotalPhys;
	}
	return 0;
}

__int64 SMR::getGlobalMemoryValue(bool percent)
{
	MEMORYSTATUSEX stat;
	stat.dwLength = sizeof(stat);
	if (::GlobalMemoryStatusEx(&stat))
	{
		if (percent) return stat.dwMemoryLoad;
		double percent = (double)stat.dwMemoryLoad * 0.01;
		__int64 bbyte = (percent * (double)stat.ullTotalPhys);
		return bbyte;
	}
	return 0;
}

__int64 SMR::getLocalMemoryValue()
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, m_processID);
	if (!hProcess)
	{
		printf("Open Process failed.\n");
		return false;
	}
	PROCESS_MEMORY_COUNTERS pmc;
	pmc.cb = sizeof(pmc);
	if (::GetProcessMemoryInfo(hProcess, &pmc, pmc.cb))
	{
		CloseHandle(hProcess);
		return pmc.WorkingSetSize;
	}
	CloseHandle(hProcess);
	return -1;
}