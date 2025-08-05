#include <windows.h>
#include <shellapi.h>
#include <chrono>
#include <thread>
#include <ctime>
#include <mmsystem.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "winmm.lib")

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT (WM_USER + 2)
#define ID_TIMER (WM_USER + 3)

// Global variables
NOTIFYICONDATA nid = {};
HWND hMainWnd;

// Forward declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void AddTrayIcon(HWND hwnd);
void RemoveTrayIcon(HWND hwnd);
void ShowContextMenu(HWND hwnd);
void showReminder();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Hide console if it's somehow allocated
    FreeConsole();

    const TCHAR CLASS_NAME[] = TEXT("StandUpReminderClass");

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    hMainWnd = CreateWindowEx(
        0, CLASS_NAME, TEXT("StandUp Reminder"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL
    );

    if (hMainWnd == NULL) {
        return 0;
    }

    // The window is created but not shown. We just need it to process messages.

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            AddTrayIcon(hwnd);
            // Set a timer to check the time every second
            SetTimer(hwnd, ID_TIMER, 1000, NULL);
            return 0;

        case WM_TIMER:
            {
                auto now = std::chrono::system_clock::now();
                std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
                struct tm localTime;
                localtime_s(&localTime, &currentTime);

                // Check if it's on the hour between 7 AM (7) and 5 PM (17)
                if (localTime.tm_hour >= 7 && localTime.tm_hour <= 17 && localTime.tm_min == 0 && localTime.tm_sec == 0) {
                    showReminder();
                }
            }
            return 0;

        case WM_TRAYICON:
            if (lParam == WM_RBUTTONUP) {
                ShowContextMenu(hwnd);
            }
            return 0;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_TRAY_EXIT) {
                DestroyWindow(hwnd);
            }
            return 0;

        case WM_DESTROY:
            RemoveTrayIcon(hwnd);
            KillTimer(hwnd, ID_TIMER);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void AddTrayIcon(HWND hwnd) {
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 100;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    // Load an icon for the tray. Create an 'app.ico' file or use a standard icon.
    nid.hIcon = (HICON)LoadImage(NULL, TEXT("app.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);
    if (nid.hIcon == NULL) {
        nid.hIcon = LoadIcon(NULL, IDI_INFORMATION); // Fallback icon
    }
    lstrcpy(nid.szTip, TEXT("Hourly Stand-Up Reminder"));
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void RemoveTrayIcon(HWND hwnd) {
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void ShowContextMenu(HWND hwnd) {
    POINT pt;
    GetCursorPos(&pt);
    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit"));
    SetForegroundWindow(hwnd); // Required for menu to work correctly
    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
    DestroyMenu(hMenu);
}

void showReminder() {
    PlaySound(TEXT("C:\\Windows\\Media\\tada.wav"), NULL, SND_FILENAME | SND_ASYNC);
    MessageBox(NULL, TEXT("It's the top of the hour! Time to stretch or take a short break."), TEXT("Hourly Reminder"), MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
}