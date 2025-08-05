#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include <Windows.h>
#pragma comment(lib, "user32.lib")

// Function to display the reminder pop-up
void showReminder() {
    MessageBox(NULL, TEXT("It's the top of the hour! Time to stretch or take a short break."), TEXT("Hourly Reminder"), MB_OK | MB_ICONINFORMATION);
}

int main() {
    // Hide the console window for a cleaner background process
    FreeConsole();

    while (true) {
        // Get the current time
        auto now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        struct tm localTime;
        localtime_s(&localTime, &currentTime);

        int currentHour = localTime.tm_hour;
        int currentMinute = localTime.tm_min;
        int currentSecond = localTime.tm_sec;

        // Check if it's on the hour between 7 AM (7) and 5 PM (17)
        if (currentHour >= 7 && currentHour <= 17 && currentMinute == 0 && currentSecond == 0) {
            showReminder();
            // Sleep for a second to avoid showing the message multiple times
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        // Calculate seconds until the next hour
        int secondsToNextHour = (59 - currentMinute) * 60 + (60 - currentSecond);
        
        // Sleep until the next hour
        std::this_thread::sleep_for(std::chrono::seconds(secondsToNextHour));
    }

    return 0;
}