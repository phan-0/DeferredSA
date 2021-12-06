#pragma once

#include "CommonD.h"

class PixelShader
{
public:
	PixelShader();
	~PixelShader();

	void CreateFromBinary(string file);
	void CreateFromFile(string file, string profile);

	void* GetShader();
private:
	void* mPixelShader;
};
