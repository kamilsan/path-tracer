#pragma once

class Vector;

bool loadPPM(const char *fileName, int &width, int &height, char*& pixels);
bool savePPM(const char *fileName, int width, int height, const char *pixels);

float saturate(float val);

void sRGBEncode(float& c);
void sRGBDecode(float& c);

void sRGBEncode(Vector& color);
void sRGBDecode(Vector& color);

//Method by Tom Duff et al.
void createOrthogonalSystem(const Vector& normal, Vector& tangent, Vector& bitangent);
