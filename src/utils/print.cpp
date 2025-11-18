#include <cstdarg>
#include "src/pch.h"
#include "print.h"

bool _enable_verbose = false;
bool is_no_entry = false;

void verbose(const char* format, ...) {
    if (_enable_verbose) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

// Progress bar class
ProgressBar::ProgressBar(int total) : total(total) {}

void ProgressBar::Add(int val) {
    current += val;
}

void ProgressBar::Update() {
    progress = (float)current / (float)total;
}
void ProgressBar::Print() {
    ProgressBar::Update();

    std::ostringstream counter;
    counter << " " << current << "/" << total << " ";
    std::string counterStr = counter.str();

    int pos = static_cast<int>(barWidth * progress);
    int mid = barWidth / 2;
    int counterStart = mid - static_cast<int>(counterStr.size()) / 2;

    std::cout << "\r[";

    for (int i = 0; i < barWidth; ++i) {
        if (i >= counterStart && i < counterStart + static_cast<int>(counterStr.size())) {
            std::cout << counterStr[i - counterStart];
        }
        else {
            std::cout << (i < pos ? "#" : "-");
        }
    }

    std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100.f) << " %";
    std::cout.flush();
}

void ProgressBar::AddAndPrint(int val) {
    ProgressBar::Add(val);
    ProgressBar::Print();
}