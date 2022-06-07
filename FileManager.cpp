#include "FileManager.h"
#include <iostream>
#include <vector>

void FileManager<float>::parseObj(std::string filePath, object<float>& obj, std::string* info)
{
	FILE* file;

	fopen_s(&file, getPath_(filePath).c_str(), "r");
	std::cout << "\nC:\\Users\\User\\source\\repos\\TunaxxEngine\\TunaxxEngine\\x64\\Debug\\standart\\objects" << '\n';
	std::cout << getPath_(filePath).c_str();
	if (file == NULL)
		throw 1;
	std::cout << '\n';

	while (true) {
		char lineBuffer[FILE_LINE_SIZE];
		char lineBufferSource[FILE_LINE_SIZE];
		int res = fscanf_s(file, "%[^\n]%*c", lineBuffer, sizeof(lineBuffer));

		int count = 0;
		if (lineBuffer[0] == 'v' && lineBuffer[1] == ' ') {
			float x, y, z;
			sscanf_s(lineBuffer, "v %f %f %f", &x, &y, &z);
			obj.vertexes.push_back(vertex<float>({ x, y, z }));
		}
		else if (lineBuffer[0] == 'v' && lineBuffer[1] == 't' && lineBuffer[2] == ' ') {
			vertex<float> vectexture({ 0, 0 });
			sscanf_s(lineBuffer, "vt %f %f", &vectexture.coordinates[0], &vectexture.coordinates[1]);
			obj.vertexesTexture.push_back(vectexture);
		}
		else if (lineBuffer[0] == 'v' && lineBuffer[1] == 'n' && lineBuffer[2] == ' ') {
			vertex<float> vecnormal({ 0, 0, 0 });
			sscanf_s(lineBuffer, "vn %f %f %f", &vecnormal.coordinates[0], &vecnormal.coordinates[1], &vecnormal.coordinates[2]);
			obj.vertexesNormal.push_back(vecnormal);
		}
		else if (lineBuffer[0] == 'f' && lineBuffer[1] == ' ') {
			int lcount = 0;
			int pos = 0;
			std::string text = lineBuffer;
			std::string space_delimiter = " ";
			int sum = 0;
			while ((pos = text.find(space_delimiter)) != std::string::npos) {
				int av, avt, avn;
				std::string splited = text.substr(0, pos);
				const char* nums = splited.c_str();
				text.erase(0, pos + space_delimiter.length());
				int maches = sscanf_s(nums, "%d/%d/%d", &av, &avt, &avn);

				if (maches <= 0)
					continue;

				int getv  = av - 1;
				int getvt = avt - 1;
				int getvn = avn - 1;

				if (sizeof(obj.vertexes[getv].coordinates))
					obj.vfaces.push_back(getv);
				if (sizeof(obj.vertexesTexture[getvt].coordinates))
					obj.vtfaces.push_back(getvt);
				if (sizeof(obj.vertexesNormal[getvn].coordinates))
					obj.vnfaces.push_back(getvn);
				count++;
				lcount++;
			}
			int av, avt, avn;
			text = text.substr(text.rfind(" ") + space_delimiter.length());
			const char* nums = text.c_str();
			int maches = sscanf_s(nums, "%d/%d/%d", &av, &avt, &avn);
			if (maches <= 0)
				continue;

			int getv = av - 1;
			int getvt = avt - 1;
			int getvn = avn - 1;

			if (sizeof(obj.vertexes[getv].coordinates))
				obj.vfaces.push_back(getv);
			if (sizeof(obj.vertexesTexture[getvt].coordinates))
				obj.vtfaces.push_back(getvt);
			if (sizeof(obj.vertexesNormal[getvn].coordinates))
				obj.vnfaces.push_back(getvn);
			count++;
			lcount++;

		}
		else if (strcmp(lineBuffer, "#") == 0) {
			char* inf = new char[FILE_LINE_SIZE];
			fgets(inf, FILE_LINE_SIZE, file);
			std::string str(inf);
			(*info) += str;
		}
		
		if (res == EOF) break;
	}
}



std::string FileManager<float>::ExePath_()
{
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	std::wstring result = std::wstring(buffer).substr(0, pos);
	std::string path( result.begin(), result.end() );
	return path;
}



template<>
std::string FileManager<float>::getPath_(std::string localFilePath)
{
	std::string path = ExePath_();
	path += localFilePath;
	return path;
}