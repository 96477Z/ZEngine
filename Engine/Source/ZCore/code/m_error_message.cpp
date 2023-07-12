#define CORE_DLLFILE

#include "m_error_message.h"

#include "f_console_output.h"

namespace zengine {
namespace internal {

/*
    Console error message and error location.
*/
CORE_DLLAPI Void ConsolePrintErrorMessage(const CChar* const error_file, const CChar* const error_funcion, 
                                          const Int32 error_line, const CChar* const error_message) noexcept {
    console::Print(
        console::ConsoleTextColour::kConsoleTextColourLightRed, 
        console::ConsoleBackgroundColour::kConsoleBackgroundColourDarkBlack,
        "�����ļ�·����%s\n��������%s\n�����У�%d\n������Ϣ��%s\n", 
        error_file, error_funcion, error_line, error_message);
}

/*
    Log error message and error location.
*/
CORE_DLLAPI Void LogErrorMessage(const CChar* const error_file, const CChar* const error_funcion,
                                 const Int32 error_line, const CChar* const error_message) noexcept {
    static ZMutex log_mutex;
    log_mutex.lock();
    //TODO(Johnasd4):The log output.

    log_mutex.unlock();
}

}//internal
}//zengine