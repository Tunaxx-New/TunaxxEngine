#pragma once
#include <vector>
#include <string>
#include "../GPUCL.h"

void Draw_Directly();

class Mesh {
private:
	int maxcount = 0;
	std::vector<face> polygons;
	std::string* extra;

	double scale[MATRIX_COUNT];
	double translate[MATRIX_COUNT];

	void cube();
	void getextra();
public:
	Mesh(char type);
	Mesh();
	void draw();
};