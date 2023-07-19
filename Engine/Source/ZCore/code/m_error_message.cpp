#define CORE_DLLFILE

#include "m_error_message.h"

#include "f_console_output.h"

namespace zengine {
namespace internal {

CORE_DLLAPI Void ConsolePrintErrorMessage(const CChar* const error_title, 
                                          const CChar* const error_file, 
                                          const CChar* const error_funcion,
                                          const Int32 error_line, 
                                          const CChar* const error_message) noexcept {
    console::Print(
        console::ConsoleOutputTextColour::kConsoleTextColourLightRed, 
        console::ConsoleOutputBackgroundColour::kConsoleBackgroundColourDarkBlack,
        "\n***** %s *****\nFile��%s\nFunction��%s\nLine��%d\nMessage��%s\n***** %s *****\n", 
        error_title, error_file, error_funcion, error_line, error_message, error_title);
}

CORE_DLLAPI Void LogErrorMessage(const CChar* const error_title,
                                 const CChar* const error_file,
                                 const CChar* const error_funcion,
                                 const Int32 error_line,
                                 const CChar* const error_message) noexcept {
    static ZMutex log_mutex;
    log_mutex.lock();
    //TODO(Johnasd4):The log output.

    log_mutex.unlock();
}

}//internal
}//zengine