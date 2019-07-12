# SelfMemoryRecycling
Simple class for any c++ projects which need to automatic recycle its memory.

## Functions

```c++
Start the timer.
  SMR::run(const long long ms, int flag=SMR_LOCAL, double maxLimit=0);
  *ms: 
      clean memory every XX ms.
  *flag: 
      SMR_LOCAL - current process
      SMR_GLOBAL - all processes
  *maxLimit: 
      1. 0.0 < maxLimit < 1.0 : using percent.
      2. 1.0 <= maxLimit : using Bytes.
Stop the timer.
  SMR::stop();

Get physical memory size.
  SMR::getPhysicalMemorySize();
  
Get physical memory size.
  SMR::getLocalMemoryValue();
  
Get global memory value.
  SMR::getGlobalMemoryValue(bool percent=true);
  *percent:
      display percent or bytes.
```

## Usage

```c++
SMR::run(3000, SMR_LOCAL, 100*SMR_MB); // Check every 3sec, if current process's memory exceeds 100 MB then clean it.  
... allocate memory ...
SMR::stop();
cout << "Local memory: " << (double)SMR::getLocalMemoryValue() / (double)SMR_MB << endl;
cout << "Global memory: " << SMR::getGlobalMemoryValue(true) << "%" << endl;
```
