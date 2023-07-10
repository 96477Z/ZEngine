#define CORE_DLLFILE

#include"f_console.h"

#include"m_error_message.h"

namespace zengine {
namespace internal {

/*
    Console error message and error location.
*/
CORE_DLLAPI Void ConsolePrintErrorMessage(const CChar* error_file, const CChar* error_funcion, const Int32 error_line,
                                     const CChar* error_message) noexcept {
    console::Print(
        console::ConsoleTextColour::kConsoleTextColourLightRed, 
        console::ConsoleBackgroundColour::kConsoleBackgroundColourDarkBlack,
        "�����ļ�·����%s\n��������%s\n�����У�%d\n������Ϣ��%s\n", 
        error_file, error_funcion, error_line, error_message);
}

/*
    Log error message and error location.
*/
CORE_DLLAPI Void LogErrorMessage(const CChar* error_file, const CChar* error_funcion, const Int32 error_line,
                                 const CChar* error_message) noexcept {
    static ZMutex log_mutex;
    log_mutex.lock();
    //TODO(Johnasd4):The log output.

    log_mutex.unlock();
}

}//internal
}//zengine