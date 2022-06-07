#pragma once
#include <Windows.h>
#include "VertexMesh.h"

//
// File Manager includes methods for reading file and parse it
//
template <typename T>
class FileManager
{
public:
	//
	// Parse obj getting info from path file obj and changes faces of Mesh
	// @throw 1 - file is NULL
	void parseObj(std::string, object<T>&, std::string*);

	//
	// Parse obj getting info from path file obj and changes faces of Mesh
	// @throw 1 - file is NULL
	// Reference: https://stackoverflow.com/questions/60297299/run-length-decompression-using-c
	void parseRLE(std::string&, std::string, std::pair<int, int>&);
private:
	//
	// https://stackoverflow.com/questions/875249/how-to-get-current-directory
	// Method gets Root for project path and cuts to last /
	std::string ExePath_();

	//
	// https://stackoverflow.com/questions/4387288/convert-stdwstring-to-const-char-in-c
	// 
	std::string getPath_(std::string);
};