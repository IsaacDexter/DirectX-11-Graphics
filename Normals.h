#pragma once
#include <DirectXMath.h>
#include <vector>
#include "Common.h"

using namespace DirectX;

void CalculateSmoothNormals(std::vector<SimpleVertex>* Vertices, std::vector<unsigned short>* Indices);
void CalculateFlatNormals(std::vector<SimpleVertex>* Vertices, std::vector<unsigned short>* Indices);