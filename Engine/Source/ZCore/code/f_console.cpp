#define CORE_DLLFILE

#include "f_console_output.h"

namespace zengine {
namespace console {

CORE_DLLAPI const Void SetConsoleOutputColour(const ConsoleOutputTextColour test_colour,
                                              const ConsoleOutputBackgroundColour background_colour) noexcept {
    internal::ZConsoleOutputSettings::Instance().set_text_colour(test_colour);
    internal::ZConsoleOutputSettings::Instance().set_background_colour(background_colour);
    //Changes the console output colour.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), 
        (ConsoleOutputColourType)test_colour | (ConsoleOutputColourType)background_colour);    
}

}//console
}//zengine
