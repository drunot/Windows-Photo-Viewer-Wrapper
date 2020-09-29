// WindowsImageViewer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <winbase.h>
#include <shlobj_core.h>
#include <stdlib.h>  
#include <wchar.h>
#include <sstream>
#include <shlobj_core.h>
#include <winreg.h>


long createKeySetValue(const HKEY & root, LPCTSTR path, LPCTSTR value, LPCTSTR data)
{
    HKEY hKey = NULL;
    long result = RegCreateKeyEx(
        root,
        path,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        NULL,
        &hKey,
        NULL);
    if (result == ERROR_SUCCESS)
    {
        result = RegSetValueEx(hKey, value, 0, REG_SZ, (LPBYTE)data, (wcslen(data) + 1) * sizeof(TCHAR));
        RegCloseKey(hKey);
        return result;
    }
    else
    {
        std::wstringstream ks;
        ks << L"Key could not be created. Return code was: " << result << " Value is: " << value;
        MessageBox(NULL, ks.str().c_str(), L"Error", MB_OK | MB_ICONWARNING | MB_DEFBUTTON1);
        return -1;
    }
}

long addFileAssosiations(LPCTSTR ext)
{
    createKeySetValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Clients\\Media\\WPV_Wrapper\\Capabilities\\FileAssociations", ext, L"AntonVS.WPVWrapper");
    std::wstringstream ks;
    ks << ext << "\\OpenWithProgids";
    createKeySetValue(HKEY_CLASSES_ROOT, ks.str().c_str(), L"AntonVS.WPVWrapper", L"");

}


long deleteTree(HKEY root, LPCTSTR path, LPCTSTR folder)
{
    HKEY hKey;
    long result = RegOpenKeyEx(root, path, 0, KEY_SET_VALUE, &hKey);
    if (result == ERROR_SUCCESS)
    {
        result = RegDeleteTree(hKey, folder);
        if (result != ERROR_SUCCESS)
        {
            std::wstringstream ks;
            ks << L"Tree could not be deleted. Return code was: " << result << " Key path is: " << path << L'\\' << folder;
            MessageBox(NULL, ks.str().c_str(), L"Error", MB_OK | MB_ICONWARNING | MB_DEFBUTTON1);
            return -1;
        }
        RegCloseKey(hKey);
        return result;
    }
    else
    {
        std::wstringstream ks;
        ks << L"Could not open tree parent folder. Return code was: " << result << " Key path is: " << path << L'\\' << folder;
        MessageBox(NULL, ks.str().c_str(), L"Error", MB_OK | MB_ICONWARNING | MB_DEFBUTTON1);
        return -1;
    }

}

long deleteValue(HKEY root, LPCTSTR path, LPCTSTR value)
{
    HKEY hKey;
    long result = RegOpenKeyEx(root, path, 0, KEY_SET_VALUE, &hKey);
    if (result == ERROR_SUCCESS)
    {
        RegDeleteValue(hKey, value);
        if (result != ERROR_SUCCESS)
        {
            std::wstringstream ks;
            ks << L"Could not delete value. Return code was: " << result << " Value is: " << value;
            MessageBox(NULL, ks.str().c_str(), L"Error", MB_OK | MB_ICONWARNING | MB_DEFBUTTON1);
            return -1;
        }
        RegCloseKey(hKey);
        return result;
    }
    else
    {
        std::wstringstream ks;
        ks << L"Could not open value parent folder. Return code was: " << result << " Value is: " << value;
        MessageBox(NULL, ks.str().c_str(), L"Error", MB_OK | MB_ICONWARNING | MB_DEFBUTTON1);
        return -1;
    }

}

long deleteFileAssosiations(LPCTSTR ext)
{
    HKEY hKey;
    std::wstringstream ks;
    ks << ext << "\\OpenWithProgids";
    long result = RegOpenKeyEx(HKEY_CLASSES_ROOT, ks.str().c_str(), 0, KEY_SET_VALUE, &hKey);
    if (result == ERROR_SUCCESS)
    {
        RegDeleteValue(hKey, L"AntonVS.WPVWrapper");
        if (result != ERROR_SUCCESS)
        {
            std::wstringstream ks;
            ks << L"Could not remove extention. Return code was: " << result << " extention is: " << ext;
            MessageBox(NULL, L"Unknown error", L"Error", MB_OK | MB_ICONWARNING | MB_DEFBUTTON1);
            return -1;
        }
        RegCloseKey(hKey);
        return result;
    }
    else
    {
        std::wstringstream ks;
        ks << L"Could not open extention parent folder. Return code was: " << result << " extention is: " << ext;
        MessageBox(NULL, ks.str().c_str(), L"Error", MB_OK | MB_ICONWARNING | MB_DEFBUTTON1);
        return -1;
    }

}


int WINAPI wWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    if (!wcscmp(lpCmdLine, L"-register"))
    {
        if (IsUserAnAdmin())
        {
            //Get wrapper path
            WCHAR szFileName[MAX_PATH];
            GetModuleFileName(NULL, szFileName, MAX_PATH);

            //Get "Program Files (x86)" path. "(x86)" ommitted on 32-bit systems.
            PWSTR PrgPath = NULL;
            SHGetKnownFolderPath(FOLDERID_ProgramFilesX86, 0, NULL, &PrgPath);


            std::wstringstream ks;
            ks << L'\"' << szFileName << L'\"' << L" \"%1\"";

            //Add app class

            createKeySetValue(HKEY_CLASSES_ROOT, L"AntonVS.WPVWrapper\\shell\\open\\command", NULL, ks.str().c_str());

            //Add descriptions and name.

            ks.str(L"");
            ks.clear();

            ks << L'\"' << PrgPath << L"\\Windows Photo Viewer\\PhotoViewer.dll\",-3069";

            createKeySetValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Clients\\Media\\WPV_Wrapper\\Capabilities", L"ApplicationDescription", ks.str().c_str());

            ks.str(L"");
            ks.clear();

            ks << L'\"' << PrgPath << L"\\Windows Photo Viewer\\PhotoViewer.dll\",-3009";

            createKeySetValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Clients\\Media\\WPV_Wrapper\\Capabilities", L"ApplicationName", ks.str().c_str());

            //File extensions
            addFileAssosiations(L".bmp");
            addFileAssosiations(L".dib");
            addFileAssosiations(L".jpg");
            addFileAssosiations(L".jpeg");
            addFileAssosiations(L".jpe");
            addFileAssosiations(L".jif");
            addFileAssosiations(L".jfif");
            addFileAssosiations(L".jfi");
            addFileAssosiations(L".jxr");
            addFileAssosiations(L".hdp");
            addFileAssosiations(L".wdp");
            addFileAssosiations(L".png");
            addFileAssosiations(L".ico");
            addFileAssosiations(L".cur");
            addFileAssosiations(L".gif");
            addFileAssosiations(L".tiff");
            addFileAssosiations(L".tif");

            //Register app

            createKeySetValue(HKEY_CLASSES_ROOT, L"SOFTWARE\\RegisteredApplications", L"WPV Wrapper", L"SOFTWARE\\Clients\\Media\\WPV_Wrapper\\Capabilities");
            return 0;
        }
        else
        {
            MessageBox(NULL, L"Admin rights needed to register", L"Error", MB_OK | MB_ICONWARNING | MB_DEFBUTTON1);
            return 0;
        }
    }
    else if (!wcscmp(lpCmdLine, L"-unregister"))
    {
        if (IsUserAnAdmin())
        {
            //File extensions
            deleteFileAssosiations(L".bmp");
            deleteFileAssosiations(L".dib");
            deleteFileAssosiations(L".jpg");
            deleteFileAssosiations(L".jpeg");
            deleteFileAssosiations(L".jpe");
            deleteFileAssosiations(L".jif");
            deleteFileAssosiations(L".jfif");
            deleteFileAssosiations(L".jfi");
            deleteFileAssosiations(L".jxr");
            deleteFileAssosiations(L".hdp");
            deleteFileAssosiations(L".wdp");
            deleteFileAssosiations(L".png");
            deleteFileAssosiations(L".ico");
            deleteFileAssosiations(L".cur");
            deleteFileAssosiations(L".gif");
            deleteFileAssosiations(L".tiff");
            deleteFileAssosiations(L".tif");

            //Delete app assosiations.
            deleteTree(HKEY_CLASSES_ROOT, NULL, L"AntonVS.WPVWrapper");
            deleteTree(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Clients\\Media",L"WPV_Wrapper");
            deleteValue(HKEY_CLASSES_ROOT, L"SOFTWARE\\RegisteredApplications", L"WPV Wrapper");
        }
        else
        {
            MessageBox(NULL, L"Admin rights needed to register", L"Error", MB_OK | MB_ICONWARNING | MB_DEFBUTTON1);
            return 0;
        }
    }
    else if (wcslen(lpCmdLine) < 1 && GetKeyState(VK_RSHIFT))
    {
        if (MessageBox(NULL, L"Do you want to unregister app so it no longer shows up as a default app for opening pictures?", L"Unregister app", MB_YESNO | MB_DEFBUTTON1) == IDYES)
        {
            //Get wrapper path
            WCHAR szFileName[MAX_PATH];
            GetModuleFileName(NULL, szFileName, MAX_PATH);
            SHELLEXECUTEINFO shExInfo = { 0 };
            shExInfo.cbSize = sizeof(shExInfo);
            shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
            shExInfo.hwnd = 0;
            shExInfo.lpVerb = L"runas";                // Operation to perform
            shExInfo.lpFile = szFileName;       // Application to start    
            shExInfo.lpParameters = L"-unregister";                  // Additional parameters
            shExInfo.lpDirectory = 0;
            shExInfo.nShow = SW_SHOW;
            shExInfo.hInstApp = 0;
            ShellExecuteEx(&shExInfo);
            return 0;
        }
    }
    else if (wcslen(lpCmdLine) < 1 && GetKeyState(VK_LSHIFT))
    {
        if (MessageBox(NULL, L"Do you want to register app as a possible default app for opening pictures?", L"Register app", MB_YESNO | MB_DEFBUTTON1) == IDYES)
        {
            //Get wrapper path
            WCHAR szFileName[MAX_PATH];
            GetModuleFileName(NULL, szFileName, MAX_PATH);
            SHELLEXECUTEINFO shExInfo = { 0 };
            shExInfo.cbSize = sizeof(shExInfo);
            shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
            shExInfo.hwnd = 0;
            shExInfo.lpVerb = L"runas";                // Operation to perform
            shExInfo.lpFile = szFileName;       // Application to start    
            shExInfo.lpParameters = L"-register";                  // Additional parameters
            shExInfo.lpDirectory = 0;
            shExInfo.nShow = SW_SHOW;
            shExInfo.hInstApp = 0;
            ShellExecuteEx(&shExInfo);
            return 0;
        }
    }
    if (lpCmdLine[0] == L'"' && lpCmdLine[wcslen(lpCmdLine) - 1] == L'"')
    {
        lpCmdLine[wcslen(lpCmdLine) - 1] = L'\0';
        lpCmdLine = &lpCmdLine[1];
    }
    PWSTR wPrgPath = NULL;
    SHGetKnownFolderPath(FOLDERID_ProgramFilesX86,0,NULL, &wPrgPath);
    std::wstringstream ss;
    ss << L"rundll32 \"" << wPrgPath << L"\\Windows Photo Viewer\\PhotoViewer.dll\", ImageView_Fullscreen " << lpCmdLine;
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
