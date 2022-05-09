#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "console_wrapper.h"

#include <Windows.h>

#include "constants.h"
#include "logger.h"
#include "winapi_helper.h"  //getLastErrorAsString

namespace utility {
ConsoleWrapper::ConsoleWrapper(bool wideChar) {
    Logger logger("ConsoleWrapper", Paths::logFilePath);

    if (AllocConsole()) {
        wideChar ? _wfreopen_s(&pFile, L"CONOUT$", L"w", stdout)
                 : freopen_s(&pFile, "CONOUT$", "w", stdout);
        ok = true;
    } else {
        DWORD errId = GetLastError();
        std::string msg = getLastErrorAsString(errId);
        logger.log(LogLevel::error, "Allocating console failed.", msg);
        // error code 5 is ACCESS_DENIED which means the console is already
        // allocated
        if (errId == 5) {
            if (FreeConsole()) {
                if (AllocConsole()) {
                    wideChar ? _wfreopen_s(&pFile, L"CONOUT$", L"w", stdout)
                             : freopen_s(&pFile, "CONOUT$", "w", stdout);
                    ok = true;
                } else {
                    errId = GetLastError();
                    msg = getLastErrorAsString(errId);
                    logger.log(LogLevel::error, "Reallocating console failed.",
                               msg);
                    ok = false;
                }
            } else {
                errId = GetLastError();
                msg = getLastErrorAsString(errId);
                logger.log(LogLevel::error, "FreeConsole() failed.", msg);
                ok = false;
            }
        } else {
            logger.log(LogLevel::error, "AllocConsole() failed", msg);
            ok = false;
        }
    }
}
ConsoleWrapper::~ConsoleWrapper() {
    if (ok) {
        fclose(pFile);
        FreeConsole();
    }
}
}  // namespace utility
