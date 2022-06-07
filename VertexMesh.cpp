#include "VertexMesh.h"
#include "Window.h"
#include "FileManager.h"
#include <iostream>

#define PI 3.1416
//For write in console
extern Window* window;
extern FileManager<float>* fileManager;
//FileManager<float>* fileManager = new FileManager<float>();

Mesh<float>::Mesh()
{
	try {
		fileManager->parseObj(FILE_OBJ_CUBE, obj_, &info_);
	}
	catch (int e) {
		switch (e)
		{
		case 1:
			window->write("Failed to read file: ");
			window->write(FILE_OBJ_CUBE);
			break;
		}
	}
}



Mesh<float>::Mesh(char type)
{

}



Mesh<float>::Mesh(std::string file)
{

}



object<float>& Mesh<float>::getObject()
{
	return obj_;
}



std::vector<std::vector<float>> Mesh<float>::getMatrixes()
{
	std::vector<std::vector<float>> matrixes;
	matrixes.push_back(scale_);
	matrixes.push_back(perspective_);
	matrixes.push_back(rotateX_);
	matrixes.push_back(rotateY_);
	matrixes.push_back(rotateZ_);
	return matrixes;
}



void Mesh<float>::rotate(int ax, int ay, int az)
{
	angleX_ += ax * PI / 180.0f;
	angleY_ += ay * PI / 180.0f;
	angleZ_ += az * PI / 180.0f;

	rotateX_[5] = cos(angleX_);
	rotateX_[6] = -sin(angleX_);
	rotateX_[9] = sin(angleX_);
	rotateX_[10] = cos(angleX_);

	rotateY_[0] = cos(angleY_);
	rotateY_[2] = sin(angleY_);
	rotateY_[8] = -sin(angleY_);
	rotateY_[10] = cos(angleY_);

	rotateZ_[0] = cos(angleZ_);
	rotateZ_[1] = -sin(angleZ_);
	rotateZ_[4] = sin(angleZ_);
	rotateZ_[5] = cos(angleZ_);
	/*
	std::vector<float> rotateX_ =
	{ 1, 0,             0           , 0,
	  0, cos(angleX_), -sin(angleX_), 0,
	  0, sin(angleX_),  cos(angleX_), 0,
	  0, 0           ,  0           , 0 };
	rotateY_ =
	{ cos(angleY_), 0, sin(angleY_), 0,
	  0, 1, 0, 0,
	  -sin(angleY_), 0, cos(angleX_), 0,
	  0, 0, 0, 0 };
	rotateZ_ =
	{ cos(angleZ_), -sin(angleZ_), 0, 0,
	  sin(angleZ_), cos(angleZ_), 0, 0,
	  0, 0, 1, 0,
	  0, 0, 0, 0 };*/
}