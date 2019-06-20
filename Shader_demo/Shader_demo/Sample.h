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
	Sample(trimesh::vec3 pos, trimesh::vec3 nor, float weg = 1.0f, int id = -1);
	trimesh::vec3 position;
	trimesh::vec3 normal;
	trimesh::vec3 pca;
	trimesh::vec3 angel;
	trimesh::vec3 angel_with_up;
	trimesh::ivec3 color;
	float weight;
	int face_id;
	int patch_id;
	float mSampleRadius;
	float weightA;
	float weightB;
};

