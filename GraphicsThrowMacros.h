#include <Windows.h>
#include <comdef.h>
#include <comip.h>
#define GFX_THROW_HR(hr) \
if (FAILED(hr)) \
{ \
    _com_error err(hr); \
    const wchar_t* errMsg = err.ErrorMessage(); \
    throw std::runtime_error( \
        std::string("DirectX function failed at ") + __FILE__ + ":" + std::to_string(__LINE__) + " - " + std::string(errMsg, errMsg + wcslen(errMsg)) \
    ); \
}