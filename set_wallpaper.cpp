#include <Windows.h>
#include <ShlObj.h>
#include <iostream>
#include <string>
#include <ctime>

bool SetWallpaperForMonitor(const std::wstring& imagePath, const std::wstring& monitorId) {
    HRESULT hr;
    IDesktopWallpaper* pDesktopWallpaper = nullptr;

    hr = CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pDesktopWallpaper));
    if (FAILED(hr)) {
        std::wcerr << L"Failed to initialize IDesktopWallpaper." << std::endl;
        return false;
    }

    hr = pDesktopWallpaper->SetWallpaper(monitorId.c_str(), imagePath.c_str());
    pDesktopWallpaper->Release();

    if (FAILED(hr)) {
        std::wcerr << L"Failed to set wallpaper for monitor. HRESULT: 0x" 
                   << std::hex << hr << std::endl;
        return false;
    }

    return true;
}

std::wstring GetWallpaperPathBasedOnTime() {
    // Get the current hour of the day
    time_t t = time(0);
    struct tm* now = localtime(&t);
    int currentHour = now->tm_hour;

    // Determine which wallpaper to use based on the time
    if (currentHour >= 0 && currentHour < 6) {
        return L"C:\\Users\\elias\\Wallpaper_Script\\Island1.jpg"; // 0-6 AM
    } else if (currentHour >= 6 && currentHour < 12) {
        return L"C:\\Users\\elias\\Wallpaper_Script\\Island2.jpg"; // 6 AM - 12 PM
    } else if (currentHour >= 12 && currentHour < 18) {
        return L"C:\\Users\\elias\\Wallpaper_Script\\Island3.jpg"; // 12 PM - 6 PM
    } else {
        return L"C:\\Users\\elias\\Wallpaper_Script\\Island4.jpg"; // 6 PM - 12 AM
    }
}
int main() {
    CoInitialize(nullptr);

    IDesktopWallpaper* pDesktopWallpaper = nullptr;
    if (FAILED(CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pDesktopWallpaper)))) {
        std::wcerr << L"Failed to create IDesktopWallpaper instance." << std::endl;
        return -1;
    }

    UINT monitorCount;
    if (FAILED(pDesktopWallpaper->GetMonitorDevicePathCount(&monitorCount))) {
        std::wcerr << L"Failed to get monitor count." << std::endl;
        pDesktopWallpaper->Release();
        return -1;
    }

    std::wstring wallpaperPath = GetWallpaperPathBasedOnTime();

    for (UINT i = 0; i < monitorCount; ++i) {
        LPWSTR monitorId = nullptr;
        if (FAILED(pDesktopWallpaper->GetMonitorDevicePathAt(i, &monitorId))) {
            std::wcerr << L"Failed to get monitor ID for monitor " << i << L"." << std::endl;
            continue;
        }


        if (!SetWallpaperForMonitor(wallpaperPath, monitorId)) {
            std::wcerr << L"Failed to set wallpaper for monitor " << i << L"." << std::endl;
        }

        CoTaskMemFree(monitorId);
    }

    pDesktopWallpaper->Release();
    CoUninitialize();

    return 0;
}
