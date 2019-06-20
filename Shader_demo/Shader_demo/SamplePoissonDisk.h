#pragma once
#include<iostream>
#include<fstream>
#include <string>
#include<sstream>
#include<vector>
#include<ctime>
#include"Sample.h"
#include "TriMesh.h"
using namespace std;

class SamplePoissonDisk
{
public:
	SamplePoissonDisk(void);
	~SamplePoissonDisk();
	SamplePoissonDisk(vector<trimesh::ivec3> face_list, vector<trimesh::vec3> position, vector<trimesh::vec3> normal);
	SamplePoissonDisk(TriMesh *mesh);

		static const double RELATIVE_RADIUS;
	public:

		bool runSampling(int numSamples);
		bool outputSample(vector<trimesh::vec3> &outPosition, vector<trimesh::vec3> &outNormal, float &outRadius);
		bool exportSample(string fileName);
		static bool loadSample(string fileName, Sample &samples);
		int clamp(int value, int min, int max);
		vector<Sample> GetSample(int numSamples, bool needCheckVis);
		bool checkVisibility(Sample &sample);

	private:

		bool calculateCDF();
		bool buildGrids(int numSamples);
		bool generateSamples(int numSamples);

		void clearUp();

	private:

		static double getPreciseRandomNumber(); // higher precision random number generator

	private:

		vector<trimesh::ivec3> mpMeshIndices;
		vector<trimesh::vec3> mpMeshVertices;
		vector<trimesh::vec3> mpMeshNormals;

	private:

		vector<double> mTriangleAreaCDF; // CDF normalized to [0..1] : # of faces
		double mTotalArea; // mesh total area
		double mSampleRadius; // sample points' minimum distance
		double mGridRadius; // grid's side length (may be larger than mSampleRadius)
		trimesh::vec3 mBBMin; // mesh bounding box corner
		trimesh::vec3 mBBMax; // mesh bounding box corner
		vector<vector<int>> mGrids; // sampleID : # of samples in grid cell : (gridXSize*gridYSize*gridZSize)
		trimesh::ivec3 mGridSize; // dimensions of grids
		vector<Sample> mSamples; // sample : # of samples
};

