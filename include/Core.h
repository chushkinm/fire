#ifndef CORE_H
#define CORE_H

#include "windows.h"

#include <string>
#include <memory>

namespace Fire {

class Window {
public:
    Window();

    bool init(const std::string& _strCaption, const std::string& _strClass,
        int _nX, int _nY, int _nWidth, int _nHeight, const HINSTANCE _hInstance);

    void run();
    void close();

protected:
    virtual void initWndClass(WNDCLASSEX& _wndClass);
    virtual void initStyle(DWORD &_dwStyle, DWORD &_dwExStyle);

    virtual void main() {}

private:
    HWND m_wndHandle;
    HINSTANCE m_hInstance;
    std::string m_strClass;

    LRESULT _wndProc(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam);
};

typedef std::shared_ptr<Window> WindowPtr;

} // Fire

#endif // CORE_H