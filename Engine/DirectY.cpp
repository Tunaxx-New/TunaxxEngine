#include <windows.h>
#include <wingdi.h>

#include <math.h>
#include <string>
#include <vector>
#include <iostream>

//Files
#include "Screen.h"
#include "../GPUCL.h"
//Thirds 3 underlevel : STRUCTS and PROCESS
using namespace std;



#define STRAIGHT_ANGLE   180;		//VAR 180*
const double PI			= 3.1416;   //VAR 3.14



/*struct vecn
{
public:
	vector<double> coord;
	int range;

	vecn(vector<double> c) : range(c.size()) {
		for (int i = 0; i < this->range; i++) {
			this->coord.push_back(c[i]);
		}
	}
	~vecn() {
		coord.shrink_to_fit();
	}

	void add(vecn v) {
		if (v.coord.size() != range) {
			printf("MATHERR: Vectors' size are different!");
			return;
		}
		for (int i = 0; i < range; i++) {
			this->coord[i] += v.coord[i];
		}
	}
	void multiply(double a) {
		for (int i = 0; i < range; i++) {
			this->coord[i] *= a;
		}
	}
	double scalarm(vecn v) {
		if (v.coord.size() != range) {
			printf("MATHERR: Vectors' size are different!");
			return 0;
		}
		double scalar = 0;
		for (int i = 0; i < range; i++) {
			scalar += this->coord[i]*v.coord[i];
		}
		return scalar;
	}
	double scalarm(double second_size, double angle, int rang) {
		if (rang != range) {
			printf("MATHERR: Vectors' size are different!");
			return 0;
		}
		double first_size = 0;
		for (int i = 0; i < range; i++) {
			first_size += this->coord[i] * this->coord[i];
		}
		first_size = sqrt(first_size);
		return first_size * second_size * cos(angle);
	}
	double scalarm(double second_size, int angle, int rang) {
		if (rang != range) {
			printf("MATHERR: Vectors' size are different!");
			return 0;
		}
		double first_size = 0;
		for (int i = 0; i < range; i++) {
			first_size += this->coord[i] * this->coord[i];
		}
		first_size = sqrt(first_size);
		double ang = (PI * angle) / STRAIGHT_ANGLE;
		return first_size * second_size * cos(ang);
	}
};*/
/*struct face {
	vector<vecn> v;
	vector<vecn> vt;
	vector<vecn> vn;
};*/

class Parser {
public:
	//https://stackoverflow.com/questions/875249/how-to-get-current-directory
	wstring ExePath() {
		TCHAR buffer[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, buffer, MAX_PATH);
		std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
		return std::wstring(buffer).substr(0, pos);
	}
	//https://stackoverflow.com/questions/4387288/convert-stdwstring-to-const-char-in-c
	const char* getPath(wstring add) {
		wstring path = ExePath();
		path += add;
		const wchar_t* str = path.c_str();

		size_t size = (wcslen(str) + 1) * sizeof(wchar_t);
		char* buffer = new char[size];
				// Count required buffer size (plus one for null-terminator).
		#ifdef __STDC_LIB_EXT1__
				// wcstombs_s is only guaranteed to be available if __STDC_LIB_EXT1__ is defined
				size_t convertedSize;
				std::wcstombs_s(&convertedSize, buffer, size, input, size);
		#else
				std::wcstombs(buffer, str, size);
		#endif

				str = NULL;
		return buffer;
	}
	void openOBJ(const char* path, vector<face> &tri, string** info) {
		FILE* file = fopen(path, "r");					//
		if (file == NULL) {								//
			printf("File is failed to open! %s", path);	// Open obj file
			throw -1;									//
		}												//

		vector<vecn>  v;								// verticles
		vector<vecn>  vt;								//
		vector<vecn>  vn;								// verticle normals

		int count = 0;
		int info_iterator = 0;

		while (1) {
			char line_buffer[128];
			int res = fscanf(file, "%s", line_buffer);

			if (strcmp(line_buffer, "v") == 0) {
				vecn verticle({ 0, 0, 0 });
				fscanf(file, "%lf %lf %lf[^\n]", &verticle.pos[0], &verticle.pos[1], &verticle.pos[2]);
				v.push_back(verticle);
			}
			else if (strcmp(line_buffer, "vt") == 0) {
				vecn vectexture({ 0, 0 });
				fscanf_s(file, "%lf %lf[^\n]", &vectexture.pos[0], &vectexture.pos[1]);
				vt.push_back(vectexture);
			}
			else if (strcmp(line_buffer, "vn") == 0) {
				vecn vecnormal({ 0, 0, 0 });
				fscanf(file, "%lf %lf %lf[^\n]", &vecnormal.pos[0], &vecnormal.pos[1], &vecnormal.pos[2]);
				vn.push_back(vecnormal);
			}
			else if (strcmp(line_buffer, "f") == 0) {
				face face;
				int lcount = 0;
				while (1) {
					int av, avt, avn;
					int maches = fscanf(file, "%d/%d/%d[^\n]", &av, &avt, &avn);
					if (maches <= 0) break;

					vecn getv = v[av - 1];
					vecn getvt = vt[avt - 1];
					vecn getvn = vn[avn - 1];

					if (sizeof(getv.pos)) {
						face.v[lcount] = getv;
					}
					if (sizeof(getvt.pos)) {
						face.vt[lcount] = getvt;
					}
					if (sizeof(getvn.pos)) {
						face.vn[lcount] = getvn;
					}
					count++;
					lcount++;
				}
				face.count = lcount;
				tri.push_back(face);
			}
			else if (strcmp(line_buffer, "#") == 0) {
				char* inf = new char[128];
				fgets(inf, 128, file);
				std::string str(inf);
				(*info)[info_iterator] = str;
				info_iterator++;
			}
			if (res == EOF) break;
		}
		v.shrink_to_fit();
		vt.shrink_to_fit();
		vn.shrink_to_fit();
	}
};
GPUFUN		  GPU{{"CLprograms/test.cl",			//GPU function
					"CLprograms/pp.cl",
					"CLprograms/matrix.cl"},
				   {"test",
					"pp",
					"matrix"}};
Parser		  parser							     ;		//Help class
extern Screen screen							     ;		//Second 2 underlevel class : SCREEN PAINTER

class Mesh
{
private:
	int maxcount;
	vector<face> polygons;
	string* extra = new string[8];

	double scale[MATRIX_COUNT];
	double translate[MATRIX_COUNT];

	void cube() {
		const char* buffer = parser.getPath(L"\\objects\\standart\\cube\\cube.obj");
		try {
			parser.openOBJ(buffer, polygons, &extra);
		}
		catch (int g) {}
	}
	void getextra() {
		cout << "Extra info: " << '\n';
		for (int i = 0; i < 8; i++) {
			cout << extra[i] << '\n';
		}
	}
	void init() {
		for (int i = 0; i < MATRIX_COUNT; ++i) { scale[i] = 0; translate[i] = 0; }
		scale[0]				   = 10;	//Scale X 0
		scale[SCALE_POSES + 1]     = 10;	//Scale Y 6
		scale[2 * SCALE_POSES + 2] = 10;	//Scale Z 11

		translate[3] = 1;	//Scale X 0
		translate[SCALE_POSES + 3] = 1;	//Scale Y 6
		translate[2 * SCALE_POSES + 3] = 1;	//Scale Z 11
	}
public:
	Mesh(char type);
	Mesh();
	void draw();
};

Mesh::Mesh(char type) {
	switch (type)
	{
	case 0:
		cube();
		break;
	default:
		break;
	}

	maxcount = 0;
	for (int i = 0; i < polygons.size(); i++)
	{
		if (maxcount < polygons[i].count) maxcount = polygons[i].count;
	}
	init();
}
Mesh::Mesh() { init(); }
void Mesh::draw() {
	//vector<face> polygon_temp(polygons);
	GPU.matrix(polygons, translate);

	for (auto const& face : polygons)
		if (maxcount >= 0)
		for (auto const& vec : face.v)
		{
			screen.pixel(vec.pos[0], vec.pos[1]);
			cout << vec.pos[1] << '\n';
		}
}

void Draw_Directly() 
{

}