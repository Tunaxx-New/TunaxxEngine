#include <iostream>
#include <vector>
#include "Screen.h"
#include "../GPUCL.h"
#include "DirectY.h"

#include <windows.h>

//First 1 underlevel

using namespace std;



const int MESH_COUNT = 1;	//Recently, it was a few memory



Screen		  screen{};		//Second 2 underlevel class : SCREEN PAINTER
extern GPUFUN GPU	  ;		//Third ? underlevel class  : DEVICE GPU

class Space
{
private:
	Mesh Meshes[MESH_COUNT];
public:
	void setMesh(Mesh mesh, int index) {
		Meshes[index] = mesh;
	}
	void DrawEveryMesh() 
	{
		for (int i = 0; i < MESH_COUNT; ++i)
		{

			Meshes[i].draw();

		}
	}
	Space() {}
};
Space Scene;
void CreateScene() 
{
	Scene.setMesh(Mesh{ 0 }, 0);
}



void Start()  {}
void SysStart(COORD screen_start, RECT window)
{
	screen.Start(screen_start, window);
	CreateScene();

	string DeviceInfo = GPU.info(true);
	screen.SetInfo(DeviceInfo, "GPU:", 1);
}



void Update() {}
void SysUpdate() {}



void Draw(HWND hWnd) 
{ 
	screen.Paint(hWnd); 
}
void DrawScreen() 
{
	Scene.DrawEveryMesh();
}
void SysDraw() {}

void OnSysDestroy() {}
void OnDestroy() 
{
	screen.undestroyed = false;
}



void SysResize(COORD screen_start, RECT window)
{
	screen.Resize(screen_start, window);
}
void SetWindowSize(COORD size) 
{
	screen.Window_size = size;
}