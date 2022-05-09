#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <iostream>
#include <fstream>
#include <Windows.h>

using namespace std;

HANDLE pipe;
HANDLE console;

std::wstring Utf8ToUtf16(std::string const& utf8) {
    std::wstring result;
    if (utf8.empty()) {
        return result;
    }
    int utf16Length = MultiByteToWideChar(CP_UTF8, 0, utf8.data(),
                                          static_cast<int>(utf8.size()), 0, 0);
    result.resize(utf16Length);
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()),
                        const_cast<wchar_t*>(result.data()), utf16Length);
    return result;
}
void setConsoleTitle() {
    // read until \r\n
    char title[1024] = {};
    char c1 = 1, c2 = 1;
    int len = 0;
    DWORD cbRead;
    BOOL success;
    do {
        success = ReadFile(pipe, &c1, sizeof(c1), &cbRead, NULL);
        if (!success && GetLastError() != ERROR_MORE_DATA) {
            strcpy_s(title, "FAILED TO SET THE TITLE");  // stop only on failure
            break;
        }
        if (c2 == '\r' && c1 == '\n') {
            title[len] = '\0';
            break;
        }
        c2 = c1;
        title[len++] = c1;
        Sleep(10);
    } while (len < sizeof(title) - 1);
    SetConsoleTitle(Utf8ToUtf16(std::string(title)).c_str());
}

void loop() {
    BOOL success;
    char buf[1024] = {};
    DWORD cbRead, cbWritten;
    do {
        success = ReadFile(pipe, buf, sizeof(buf), &cbRead, NULL);
        if (!success && GetLastError() != ERROR_MORE_DATA) {
            break;
        }
        if (!WriteFile(console, buf, cbRead, &cbWritten, NULL)) {
            break;
        }
        Sleep(10);
    } while (true);
}
int main(int argc, char** argv) {
    if (argc < 3) {
        return 1;
    }
    string pipeName(argv[1]);
    ofstream logFile(argv[2]);

    while (1) {
        pipe = CreateFile(Utf8ToUtf16(pipeName).c_str(), GENERIC_READ, 0, NULL,
                          OPEN_EXISTING, 0, NULL);
        // break if handle is valid
        if (pipe != INVALID_HANDLE_VALUE) {
            break;
        }
        if (GetLastError() != ERROR_PIPE_BUSY) {
            logFile << "Cannot open pipe(1).\n";
            return 1;
        }
        cout << "Waiting for a pipe...\n";
        if (!WaitNamedPipe(Utf8ToUtf16(pipeName).c_str(), 20000)) {
            logFile << "Cannot open pipe(2).\n";
            return 1;
        }
        Sleep(500);
    }
    console = GetStdHandle(STD_OUTPUT_HANDLE);

    setConsoleTitle();
    loop();
    CloseHandle(pipe);
}