#include "Core.h"

#include <map>
#include <functional>

namespace Fire {

static std::map<HWND, std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>> windows;

static LRESULT CALLBACK _wndProc(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam) {
    const auto iWnd = windows.find(_hWnd);
    if (iWnd == windows.end())
        return DefWindowProc(_hWnd, _uMsg, _wParam, _lParam);

    const auto wndProc = iWnd->second;
    if (_uMsg == WM_DESTROY)
        windows.erase(iWnd);

    return wndProc(_hWnd, _uMsg, _wParam, _lParam);
}

Window::Window() : m_wndHandle(0) {}

Window::Window(const std::string& _strCaption, const std::string& _strClass,
    int _nX, int _nY, int _nWidth, int _nHeight, const HINSTANCE _hInstance)
{
    init(_strCaption, _strClass, _nX, _nY, _nWidth, _nHeight, _hInstance);
}

bool Window::init(const std::string& _strCaption, const std::string& _strClass,
    int _nX, int _nY, int _nWidth, int _nHeight, const HINSTANCE _hInstance)
{
    m_hInstance = _hInstance;
    m_strClass = _strClass;

    WNDCLASSEX wndClass;
    initWndClass(wndClass);
    if (RegisterClassEx(&wndClass) == NULL)
        return false;

    DWORD dwStyle, dwExStyle;
    initStyle(dwStyle, dwExStyle);

    m_wndHandle = CreateWindowEx(dwExStyle, _strClass.c_str(), _strCaption.c_str(),
        dwStyle, _nX, _nY, _nWidth, _nHeight, 0, 0, _hInstance, NULL);
    if (m_wndHandle == 0)
        return false;

    return true;
}

void Window::run() {
    const auto wndProc = [&] (HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
        { return this->_wndProc(_hWnd, _uMsg, _wParam, _lParam); };
    windows.insert(std::make_pair(m_wndHandle, wndProc));

    ShowWindow(m_wndHandle, SW_SHOW);
    SetForegroundWindow(m_wndHandle);
    SetFocus(m_wndHandle);

    MSG msg;
    while (1) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message != WM_QUIT) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
                break;
        }
        else
            main();
    }
}

void Window::close() {
    PostMessage(m_wndHandle, WM_QUIT, NULL, NULL);
}

void Window::initWndClass(WNDCLASSEX& _wndClass) {
    _wndClass.style = CS_VREDRAW | CS_HREDRAW;
    _wndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
    _wndClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
    _wndClass.hCursor = LoadIcon(m_hInstance, IDC_ARROW);
    _wndClass.hInstance = m_hInstance;
    _wndClass.cbSize = sizeof(WNDCLASSEX);
    _wndClass.cbClsExtra = 0;
    _wndClass.cbWndExtra = 0;
    _wndClass.hbrBackground = HBRUSH(COLOR_BACKGROUND);
    _wndClass.lpszMenuName = NULL;
    _wndClass.lpszClassName = m_strClass.c_str();
    _wndClass.lpfnWndProc = &Fire::_wndProc;
}

void Window::initStyle(DWORD &_dwStyle, DWORD &_dwExStyle) {
    _dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    _dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
}

LRESULT Window::_wndProc(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam) {
    switch (_uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return NULL;
        default:
            return DefWindowProc(_hWnd, _uMsg, _wParam, _lParam);
    }
}

} // Fire