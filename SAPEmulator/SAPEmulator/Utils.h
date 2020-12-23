#pragma once
#include <filesystem>
#include <string>
#include <Windows.h>
#include <iostream>
#include <fstream>

std::wstring OpenFileName(wchar_t* filter = (wchar_t*)L"All Files (*.*)\0*.*\0", HWND owner = NULL) {
	OPENFILENAME ofn;
	wchar_t fileName[MAX_PATH] = L"";
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"";

	std::wstring fileNameStr;
	
	if (GetOpenFileName(&ofn))
		fileNameStr = fileName;

	return fileNameStr;
}
namespace fs = std::filesystem;
size_t ReadFile(std::wstring path, char*& out) {
	std::ifstream stream = std::ifstream(path);
	size_t size = fs::file_size(fs::path(path));
	out = new char[size];
	int i = 0;
	while (!stream.eof()) {
		stream.get(out[i++]);//because getting the entire file at once didn't work
	}
	stream.close();
	return size;
}