#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "console.h"

#include "constants.h"
#include "logger.h"
#include "types.h"
#include "winapi_helper.h"

namespace utility {

Console::Console(std::string consoleTitle, std::string const& clientPath)
    : pipe(INVALID_HANDLE_VALUE) {
    std::string logPath =
        clientPath.substr(0, clientPath.find_last_of('.')) + "Log.txt";
    if (create(consoleTitle, clientPath, logPath)) {
        Console::ok = true;
    }
}

bool Console::create(std::string consoleTitle, std::string const& clientPath,
                     std::string const& logPath) {
    Logger logger("Console::create", Paths::logFilePath);
    if (consoleTitle.find("\r\n") == std::string::npos) {
        consoleTitle.append("\r\n");
    }
    this->consoleTitle = consoleTitle;
    // ensure no pipe connected
    if (pipe != INVALID_HANDLE_VALUE) {
        DisconnectNamedPipe(pipe);
        CloseHandle(pipe);
        pipe = INVALID_HANDLE_VALUE;
    }
    pipeName.append("\\\\.\\pipe\\")
        .append("ConsolePipe_")
        .append(std::to_string(GetTickCount64()));

    // pipeName = "\\\\.\\pipe\\ConsolePipe_54054296"; //JUST FOR DEBUGGING

    pipe = CreateNamedPipe(Utf8ToUtf16(pipeName).c_str(), PIPE_ACCESS_OUTBOUND,
                           PIPE_TYPE_MESSAGE | PIPE_READMODE_BYTE | PIPE_WAIT,
                           1, 4096, 0, 1, NULL);

    if (pipe == INVALID_HANDLE_VALUE) {
        logger.log(LogLevel::error,
                   "Constructor of Console failed: ", consoleTitle,
                   "Cannot create named pipe.");
        return false;
    }

    STARTUPINFO si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    std::string cmdLine = clientPath + " " + pipeName + " " + logPath;
    BOOL cpStatus =
        CreateProcess(NULL, Utf8ToUtf16(cmdLine).data(), NULL, NULL, FALSE,
                      CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

    if (!cpStatus) {
        auto errorCode = GetLastError();
        logger.log(LogLevel::error,
                   "Cannot create console process:", consoleTitle,
                   getLastErrorAsString(errorCode), "command line:", cmdLine);
        CloseHandle(pipe);
        pipe = INVALID_HANDLE_VALUE;
        return false;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    BOOL connected = ConnectNamedPipe(pipe, NULL);
    if (!connected) {
        auto errorCode = GetLastError();
        logger.log(LogLevel::error, "Cannot connect named pipe:", consoleTitle,
                   getLastErrorAsString(errorCode));
        CloseHandle(pipe);
        pipe = INVALID_HANDLE_VALUE;
        return false;
    }

    // set title
    print(consoleTitle);
    return true;
}
bool Console::WriteFileInternal(HANDLE pipe, void const* buffer,
                                unsigned long numberOfBytesToWrite,
                                unsigned long* numberOfBytesWritten) {
    return WriteFile(pipe, buffer, numberOfBytesToWrite, numberOfBytesWritten,
                     NULL);
}
Console::~Console() {
    if (pipe != NULL && pipe != INVALID_HANDLE_VALUE) {
        FlushFileBuffers(pipe);
        DisconnectNamedPipe(pipe);
        CloseHandle(pipe);
        pipe = INVALID_HANDLE_VALUE;
    }
}

void Console::enterCritialSection() {
    while (InterlockedCompareExchange(&lock, 1, 0) != 0) {
        Sleep(0);
    };
}

void Console::leaveCritialSection() { lock = 0; }
void Console::flush() {
    if (pipe != NULL && pipe != INVALID_HANDLE_VALUE) {
        FlushFileBuffers(pipe);
    }
}
}  // namespace utility
