// WindowsImageViewer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "resource.h"
#include <windows.h>
#include <winbase.h>
#include <shlobj_core.h>
#include <stdlib.h>  
#include <wchar.h>
#include <sstream>

int WINAPI wWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    if (lpCmdLine[0] == L'"' && lpCmdLine[wcslen(lpCmdLine) - 1] == L'"')
    {
        lpCmdLine[wcslen(lpCmdLine) - 1] = L'\0';
        lpCmdLine = &lpCmdLine[1];
    }
    PWSTR wPrgPath = NULL;
    SHGetKnownFolderPath(FOLDERID_ProgramFilesX86,0,NULL, &wPrgPath);
    std::wstringstream ss;
    ss << L"rundll32 \"" << wPrgPath << L"\\Windows Photo Viewer\\PhotoViewer.dll\", ImageView_Fullscreen " << lpCmdLine;
    //MessageBox(NULL, ss.str().c_str(), wPrgPath,  MB_OK );
    size_t runSize = ss.str().length()+1;
    size_t mehWindowsSecureFunctionsSucks = 0;
    char* runStr = new char[runSize];
    wcstombs_s(&mehWindowsSecureFunctionsSucks,runStr, runSize, ss.str().c_str(), runSize);

    int error = WinExec(runStr,3);
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
