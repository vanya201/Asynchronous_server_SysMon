#include <windows.h>
#include <memory>
#include <sstream>

class SystemInfo {
public:
	SystemInfo() = default;

  struct Info
  {
    double cpuUsage;
    double memoryUsage;
	double diskUsage;
  };

public:
  Info get_system_usage()
  {
	return {get_cpu_usage(), get_memory_usage(), get_disk_usage()};
  }

private:
  double get_cpu_usage()
  {
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
      ULARGE_INTEGER idle, kernel, user;
      idle.LowPart = idleTime.dwLowDateTime;
      idle.HighPart = idleTime.dwHighDateTime;
      kernel.LowPart = kernelTime.dwLowDateTime;
      kernel.HighPart = kernelTime.dwHighDateTime;
      user.LowPart = userTime.dwLowDateTime;
      user.HighPart = userTime.dwHighDateTime;

      ULONGLONG total = (kernel.QuadPart + user.QuadPart);
      ULONGLONG idleTimeTotal = idle.QuadPart;
      return ((total - idleTimeTotal) * 100.0) / total;
    }
    return 0.0;
  }

  double get_memory_usage()
  {
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(MEMORYSTATUSEX);

    if (GlobalMemoryStatusEx(&statex))
      return (statex.ullTotalPhys - statex.ullAvailPhys) / 1024.0 / 1024.0;

    return 0.0;
  }

  double get_disk_usage()
  {
  
    char diskPath[] = "C:\\";

    ULARGE_INTEGER freeBytes, totalBytes, availableBytes;

    if (GetDiskFreeSpaceExA(diskPath, &availableBytes, &totalBytes, &freeBytes)) {
      double total = totalBytes.QuadPart / 1024.0 / 1024.0; 
      double free = freeBytes.QuadPart / 1024.0 / 1024.0;
      return total - free;
    }
    return 0.0;
  }
};