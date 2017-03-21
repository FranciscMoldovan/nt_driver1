#include "dll1.h"

__forceinline const char *lastchars(const char *file, int k)
{
    if ((int)strlen(file)<k)
        return file;
    else
        return file+strlen(file)-k;
}

DLL_API
void
LogPrint2(
    char*   SourceFile,
    __int32 SourceLine,
    char*   FunctionName,
    const char* Format,
    ...
)
{
    va_list argList;
    char buffer[65000];
    int headLen, bodyLen;
    DWORD written;

    buffer[sizeof(buffer)-1] = 0;
    bodyLen = 0;
    written = 0;

    if ((NULL != SourceFile) && (NULL != FunctionName))
    {
        headLen = sprintf_s(buffer, sizeof(buffer)-1, "%14s, %4d, %s: ", 
                        lastchars(SourceFile, 14), SourceLine, FunctionName);
    }
    else
    {
        headLen = 0;
    }

    // print variable parameter list
    va_start(argList, Format);
    bodyLen = vsprintf_s(&(buffer[headLen]), sizeof(buffer)-1-headLen, Format, argList);

    OutputDebugStringA(buffer);
}