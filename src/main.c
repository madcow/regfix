#include <MinHook.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Configure suffix for registry subkey
#define SUBKEY_SUFFIX  L"Vanilla"

// No need to change these internal values
#define SUBKEY_ORIGINAL_SUBDIR       L"Client"
#define SUBKEY_ORIGINAL_BASEPATH     L"Software\\Blizzard Entertainment\\World of Warcraft"
#define SUBKEY_ORIGINAL_FULL         SUBKEY_ORIGINAL_BASEPATH L"\\" SUBKEY_ORIGINAL_SUBDIR
#define SUBKEY_REPLACEMENT_BASEPATH  SUBKEY_ORIGINAL_BASEPATH L" " SUBKEY_SUFFIX
#define SUBKEY_REPLACEMENT_FULL      SUBKEY_REPLACEMENT_BASEPATH L"\\" SUBKEY_ORIGINAL_SUBDIR

// Original RegOpenKeyExW function pointer
typedef LSTATUS (WINAPI *RegOpenKeyExW_t) (HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
RegOpenKeyExW_t OriginalRegOpenKeyExW = NULL;

// Original RegCreateKeyExW function pointer
typedef LSTATUS (WINAPI *RegCreateKeyExW_t) (HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions,
	REGSAM samDesired, const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition);
RegCreateKeyExW_t OriginalRegCreateKeyExW = NULL;

// Custom RegOpenKeyExW: Intercept registry calls and rewrite relevant subkeys
LSTATUS WINAPI CustomRegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
	if (lstrcmpW(lpSubKey, SUBKEY_ORIGINAL_BASEPATH) == 0) {
		lpSubKey = SUBKEY_REPLACEMENT_BASEPATH;
	} else if (lstrcmpW(lpSubKey, SUBKEY_ORIGINAL_FULL) == 0) {
		lpSubKey = SUBKEY_REPLACEMENT_FULL;
	}

	return OriginalRegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

// Custom RegCreateKeyExW: Intercept registry calls and rewrite relevant subkeys
LSTATUS WINAPI CustomRegCreateKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions,
	REGSAM samDesired, const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition)
{
	if (lstrcmpW(lpSubKey, SUBKEY_ORIGINAL_BASEPATH) == 0) {
		lpSubKey = SUBKEY_REPLACEMENT_BASEPATH;
	} else if (lstrcmpW(lpSubKey, SUBKEY_ORIGINAL_FULL) == 0) {
		lpSubKey = SUBKEY_REPLACEMENT_FULL;
	}

	return OriginalRegCreateKeyExW(hKey, lpSubKey, Reserved, lpClass, dwOptions,
		samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

// DLL entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason) {

	// Attach handler
	case DLL_PROCESS_ATTACH:
		if (MH_Initialize() != MH_OK) {
			return FALSE;
		}

		if (MH_CreateHookApi(L"advapi32", "RegOpenKeyExW", &CustomRegOpenKeyExW, (LPVOID*) &OriginalRegOpenKeyExW) != MH_OK) {
			return FALSE;
		}

		if (MH_CreateHookApi(L"advapi32", "RegCreateKeyExW", &CustomRegCreateKeyExW, (LPVOID*) &OriginalRegCreateKeyExW) != MH_OK) {
			return FALSE;
		}

		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
			return FALSE;
		}

		break;

	// Detach handler
	case DLL_PROCESS_DETACH:
		// If the DLL's entry-point function returns FALSE following a DLL_PROCESS_ATTACH
		// notification, it receives a DLL_PROCESS_DETACH notification and the DLL is
		// unloaded immediately. So all clean up code can be added here.
		MH_DisableHook(MH_ALL_HOOKS);
		MH_Uninitialize();
		break;
	}

	return TRUE;
}