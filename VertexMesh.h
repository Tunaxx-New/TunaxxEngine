#pragma once
#include "Constants.h"
#include <vector>
#include <string>

typedef signed int cl_int;

//
// Standart vertex for cl needs
//
template <typename T>
union vertex
{
	T coordinates[OBJ_COORD_COUNT];
};

//
// Standart face for cl needs
//

template <typename T>
struct object
{
	std::vector<vertex<T>> vertexes;
	std::vector<vertex<T>> vertexesTexture;
	std::vector<vertex<T>> vertexesNormal;

	std::vector<int> vfaces;
	std::vector<int> vtfaces;
	std::vector<int> vnfaces;
};

//
// This class is standart Raster figure with vertexes, faces and other options of .obj
//
template <typename T>
class Mesh
{
public:
	//
	// Constructor for cube
	//
	Mesh();

	//
	// Constructor for cube
	//
	Mesh(char);

	//
	// Constructor for cube
	//
	Mesh(std::string);

	//
	// Method that returns referense to faces
	//
	object<T>& getObject();

	//
	// Getting all matrixes
	//
	std::vector<std::vector<float>> getMatrixes();

	//
	// Getting all matrixes
	//
	void rotate(int, int, int);

private:
	//
	// Faces of object
	//
	object<T> obj_;

	//
	// Info string about object
	//
	std::string info_;

	//
	// Rotates x, y, z;
	//
	float angleX_ = 0;
	float angleY_ = 0;
	float angleZ_ = 0;

	//
	// Scale Matrix
	//
	std::vector<float> scale_ =
	{ 10, 0, 0, 0,
	  0, 10, 0, 0,
	  0, 0, 10, 0,
	  0, 0, 0, 0 };



	//
	// Perspective Matrix
	//
	std::vector<float> perspective_ =
	{ 1, 0, 0, 0,
	  0, 1, 0, 0,
	  0, 0, 1, 0,
	  0, 0, 0, 0 };

	//
	// Rotate X Matrix
	//
	std::vector<float> rotateX_ =
	{ 1, 0,             0           , 0,
	  0, cos(angleX_), -sin(angleX_), 0,
	  0, sin(angleX_),  cos(angleX_), 0,
	  0, 0           ,  0           , 0 };

	//
	// Rotate Y Matrix
	//
	std::vector<float> rotateY_ =
	{ cos(angleY_), 0, sin(angleY_), 0,
	  0, 1, 0, 0,
	  -sin(angleY_), 0, cos(angleX_), 0,
	  0, 0, 0, 0 };

	//
	// Rotate Z Matrix
	//
	std::vector<float> rotateZ_ =
	{ cos(angleZ_), -sin(angleZ_), 0, 0,
	  sin(angleZ_), cos(angleZ_), 0, 0,
	  0, 0, 1, 0,
	  0, 0, 0, 0 };
};