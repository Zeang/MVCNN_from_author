#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include<iostream>
#include<vector>
#include "TriMesh.h"
using namespace std;
using namespace trimesh;
class Sample
{
public:
	Sample(void);
	~Sample(void);
	Sample(vec3 pos, vec3 nor, float weg = 1.0f, int id = -1);
	vec3 position;
	vec3 normal;
	vec3 pca;
	vec3 angel;
	vec3 angel_with_up;
	ivec3 color;
	float weight;
	int face_id;
	int patch_id;
	float mSampleRadius;
	float weightA;
	float weightB;
	bool flag;
	double prob;
};

