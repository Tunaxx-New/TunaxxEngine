#include <windows.h>
#include "../GPUCL.h"
#pragma once
void SysStart(COORD, RECT);
void SysUpdate();
void Start();
void Update();
void OnDestroy();
void OnSysDestroy();
void Draw(HWND);
void SysDraw();
void SetWindowSize(COORD);
void DrawScreen();
void SysResize(COORD, RECT);