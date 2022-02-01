#include "framework.h"
#include "main.h"
#include "Engine/Engine.h"

//Enrty 0 underlevel

//Preprocessors "constants"
    //WINDOW
#define MAX_LOADSTRING 32
#define START_WINDOW_X 0
#define START_WINDOW_Y 0
#define WINDOW_BORDERS 7
#define WINDOW_CAPTION 30
    //CONSOLE
#define CONSOLE_TEXT_ATTRIBUTE 10

//FPS variables
const WPARAM   TIMER_FPS     = 1;                   //timer type
const unsigned FPS_GET_SPEED = 1000;                //speed of fps
unsigned       frames        = 0;                   //frame count
unsigned       FPS           = 0;                   //FPS variable

//Window tools
const HBRUSH    caption_brush = CreateSolidBrush(RGB(25, 25, 25)); //Brush for caption background
const COLORREF  captext_brush = RGB(255, 255, 255);                //Brush for caption text

//Window classes
HANDLE     hConsole                     ; //Console handler
HINSTANCE  hInst                        ; //Instance handler
WCHAR      sTitle       [MAX_LOADSTRING]; //Title - название окна
WCHAR      sWindowClass [MAX_LOADSTRING]; //Window class type
WCHAR      sConsoleTitle[MAX_LOADSTRING]; //Console Title

//Window rectangles
RECT       desktop_rect     ;                                              
RECT       window_rect      ;
SMALL_RECT console_rect     ;
RECT       saved_window_rect;

//BOOL types
BOOL       run                 = true;  //Main bool
bool       fullscreen_switcher = true;  //Fullscreen mode bool

ATOM             MyRegisterClass (HINSTANCE hInstance)       ;   //WORD type register class window   #function:INIT  Sys
BOOL             InitInstance    (HINSTANCE, int)            ;   //Initialize window                 #function:INIT  Sys
LRESULT CALLBACK WndProc         (HWND, UINT, WPARAM, LPARAM);   //Handle messagin resukt            #function:CYCLE SyS
static  string   WC2LC           (const wstring&)            ;   //from WCHAR* to STRING (and LPSRT) #function:TECH  ReF
void             GetWindowRects  (HWND)                      ;   //Getting window's rectangles       #function:GET   ByA
void             GetConsole      ()                          ;   //Execute console window            #function:INIT  ByA



// Начало, запускается окно и MESSAGE(msg) отправляется на обработку APIENTRY-WINAPI
int APIENTRY wWinMain(_In_     HINSTANCE hInstance,    //for exe instance
                      _In_opt_ HINSTANCE hPrevInstance,//used in 16-bit window equals 0
                      _In_     LPWSTR    lpCmdLine,    //Command line arguments
                      _In_     int       nCmdShow)     //minimized or maximized or shown normally
{
    //Using parameters
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    //Loading strings from APP
    LoadStringW(hInstance, IDS_APP_TITLE, sTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RAYTRACINGLAB, sWindowClass, MAX_LOADSTRING);
    LoadStringW(hInstance, IDS_CONSOLE_TITLE, sConsoleTitle, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RAYTRACINGLAB));

    MSG msg;
    msg.message = (~WM_QUIT);
    SysStart(COORD{ 0, WINDOW_CAPTION }, window_rect);
    while (run)                                                 //Жизненный цикл программы
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            SysUpdate();
            InvalidateRect(msg.hwnd, NULL, false);
            Draw(msg.hwnd);
            //cout << msg.message << '\n';
        }
    }

    OnSysDestroy();
    return (int) msg.wParam;
}



static string WC2LC(const wstring& src) // https://coderedirect.com/questions/277249/how-to-convert-from-wchar-t-to-lpstr
{
    const int size = ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, NULL, 0, 0, NULL);
    vector<char> buf(size);
    ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, &buf[0], size, 0, NULL);
    return string(&buf[0]);
}
void GetWindowRects(HWND main_hWnd) {
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop_rect);
    GetWindowRect(main_hWnd, &window_rect);
}
void GetConsole() {                                                     //https://cpp.hotexamples.com/ru/examples/-/-/AllocConsole/cpp-allocconsole-function-examples.html

    console_rect.Bottom = window_rect.bottom;
    console_rect.Right = desktop_rect.right - window_rect.right;

    COORD csize;
    csize.X = console_rect.Right - console_rect.Left;
    csize.Y = console_rect.Bottom - console_rect.Top;

    AllocConsole();

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    void* ptr;
    ptr = freopen("CONOUT$", "w", stdout);

    CONSOLE_FONT_INFO GETFONT;
    GetCurrentConsoleFont(hConsole, true, &GETFONT);
    COORD font_size = GetConsoleFontSize(hConsole, GETFONT.nFont);
    console_rect.Right /= font_size.X;
    console_rect.Bottom /= font_size.Y;

    string s = WC2LC(sConsoleTitle);
    LPCSTR lpStr = s.c_str();
    SetConsoleTitleA(lpStr);
    SetConsoleWindowInfo(hConsole, true, &console_rect);
    SetConsoleScreenBufferSize(hConsole, csize);

    HWND consoleWindow = GetConsoleWindow();
    SetWindowPos(consoleWindow, 0, window_rect.right - WINDOW_BORDERS, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    SetConsoleTextAttribute(hConsole, CONSOLE_TEXT_ATTRIBUTE);
}



//Initailization
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RAYTRACINGLAB));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+3);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_RAYTRACINGLAB);
    wcex.lpszClassName  = sWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, 
                  int       nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(sWindowClass, sTitle, WS_OVERLAPPEDWINDOW,
       START_WINDOW_X, START_WINDOW_Y, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   SetWindowLong(hWnd, GWL_STYLE, WS_BORDER);
   SetWindowLong(hWnd, GWL_STYLE, WS_SIZEBOX);
   SetWindowLong(hWnd, GWL_STYLE, COLOR_ACTIVECAPTION);

   if (!hWnd)
   {
      return FALSE;
   }
   
   GetWindowRects(hWnd);

   GetConsole();

   SetWindowSize(COORD{ (short)window_rect.right, (short)window_rect.bottom });

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}



//Function messages cycle
LRESULT CALLBACK WndProc(HWND   hWnd, 
                         UINT   message, 
                         WPARAM wParam, 
                         LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        Start();
        SetTimer(hWnd,
            TIMER_FPS,
            1000,
            (TIMERPROC)NULL);
    }
    case WM_ERASEBKGND:
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
    case WM_KEYDOWN:
    {
        switch (wParam) {
        case VK_ESCAPE:
        {
            OnDestroy();
            SendMessage(hWnd, WM_CLOSE, wParam, lParam);
            run = false;
        }
        break;
        case VK_F11:
        {
            if (fullscreen_switcher) {
                saved_window_rect = window_rect;
                window_rect.right = desktop_rect.right;
                SetWindowPos(hWnd, NULL, 0, 0, desktop_rect.right, desktop_rect.bottom, NULL);
                SysResize(COORD{ 0, WINDOW_CAPTION }, desktop_rect);
            }
            else {
                window_rect = saved_window_rect;
                SetWindowPos(hWnd, NULL, 0, 0, window_rect.right, window_rect.bottom, NULL);
                SysResize(COORD{ 0, WINDOW_CAPTION }, window_rect);
            }
            fullscreen_switcher = !fullscreen_switcher;
        }
        break;
        }
        break;
    }
    break;
    case WM_MOUSEMOVE:
    {

    }
    break;
    case WM_TIMER:
    {
        switch (wParam) {
        case (TIMER_FPS):
        {
            FPS = frames;
            SetTimer(hWnd, TIMER_FPS, FPS_GET_SPEED, (TIMERPROC)NULL);
            frames = 0;
        }
        break;
        }
    }
    break;
    case WM_PAINT:
    {

    }
    break;
    case WM_NCPAINT:
    {
        PAINTSTRUCT ps;
        RECT rcCap = { 0, 0, window_rect.right, WINDOW_CAPTION };
        BeginPaint(hWnd, &ps);

        SysDraw();

        SetBkMode(ps.hdc, TRANSPARENT);

        #ifdef _DEBUG
        FillRect(ps.hdc, &rcCap, caption_brush);
        #endif
        SetTextColor(ps.hdc, captext_brush);
        DrawText(ps.hdc, sTitle, -1, &rcCap, DT_SINGLELINE | DT_VCENTER);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    default:
        SysUpdate();
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}