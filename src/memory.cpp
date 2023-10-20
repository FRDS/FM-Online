#include <string>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <Psapi.h>

using namespace std;
using std::string;

char *pMem;
DWORD procID;
HANDLE hFile;

void ReadMem(uintptr_t psxAddr, void *read, int size)
{
   memcpy(read, &pMem[psxAddr], size);
}

void WriteMem(uintptr_t psxAddr, void *write, int size)
{
   memcpy(&pMem[psxAddr], write, size);
}

// uintptr_t GetPSXRAM(uintptr_t start, int size)
// {
//   char *duck = new char[size];
//   const char gameExe[] = "SLUS_014.11;1";
//   const int gameExePSXOffset = 0x9244;
//   ReadMem(start, &duck[0], size);
//   for (int i = 0; i < size; i++)
//   {
//     if (!memcmp(&gameExe[0], &duck[i], sizeof(gameExe)))
//     {
//       baseAddress = start + i - gameExePSXOffset;
//       cout << "PSX RAM found at " << baseAddress << endl;
//       break;
//     }
//   }
//   delete[] duck;
//   return baseAddress;
// }

// Function copied from https://stackoverflow.com/questions/48208782/read-memory-of-process-c
// uintptr_t FindBaseAddress(void)
// {
//   uintptr_t ModuleBaseAddress = 0;
//   int ModuleSize = 0;
//   HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, procID);
//   if (hSnapshot != INVALID_HANDLE_VALUE)
//   {
//     MODULEENTRY32 ModuleEntry32;
//     ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
//     if (Module32First(hSnapshot, &ModuleEntry32))
//     {
//       do
//       {
//         char *moduleName = ModuleEntry32.szModule;
//         if (strstr(moduleName, "duckstation") != nullptr && strstr(moduleName, ".exe") != nullptr)
//         {
//           ModuleBaseAddress = (uintptr_t)ModuleEntry32.modBaseAddr;
//           ModuleSize = (int)ModuleEntry32.modBaseSize;
//           cout << "Module found at " << ModuleBaseAddress << endl;
//           break;
//         }
//       } while (Module32Next(hSnapshot, &ModuleEntry32));
//     }
//     CloseHandle(hSnapshot);
//   } else {
//     cout << "Error creating snapshot: " << GetLastError() << endl;
//   }
//   return GetPSXRAM(ModuleBaseAddress, ModuleSize);
// }

bool GetProcessSettings()
{
  const wchar_t *windowTitle = L"Yu-Gi-Oh! Forbidden Memories (USA)";

  HWND hwnd = FindWindowW(NULL, windowTitle);
  if (hwnd == NULL)
  {
    cerr << "Process not found." << endl;
    return false;
  }
  GetWindowThreadProcessId(hwnd, &procID);

  cout << "Process found, PID: " << procID << endl;

  wstring processNameT = TEXT("duckstation_");
  processNameT += to_wstring(procID);

  const unsigned int size = 0x800000;
  hFile = OpenFileMappingW(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, processNameT.c_str());
  pMem = (char *)MapViewOfFile(hFile, FILE_MAP_READ | FILE_MAP_WRITE, FALSE, FALSE, size);
  if ((hFile == nullptr) || (pMem == NULL))
  {
    cerr << "Shared Memory Failed." << endl;
    return false;
  }
  return true;
}