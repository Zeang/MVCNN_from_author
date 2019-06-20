#include "TheaKDTreeHelper.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include "TriMesh.h"
#include <time.h>
#include<algorithm>
#include"SamplePoissonDisk.h"
#include "Eigen/Dense"
#include "Eigen/LU"

#include <direct.h>
using namespace std;
using namespace trimesh;
#define PI 3.1415926

vector<vec> kmedoidsClusteringNew(vector<vec> inputData, int kCluster = 3)
{
	vector<int> kCenters;
	// initial center
	while (kCenters.size() < kCluster){
		int index = rand() % inputData.size();
		bool isNew = true;
		for (auto center : kCenters){
			if (center == index){
				isNew = false;
				break;
			}
		}
		if (isNew) kCenters.push_back(index);
	}
	// clustering
	for (int i = 0; i < 100; i++)
	{
		// check group
		vector<vector<int>> kgroups(kCluster);
		for (int j = 0; j < inputData.size(); j++)
		{
			float minDis = FLT_MAX;
			float minIndex = -1;
			for (int c = 0; c < kCluster; c++)
			{
				float d = 1.0f - (inputData[j] DOT inputData[kCenters[c]]);
				if (d < minDis)
				{
					minDis = d;
					minIndex = c;
				}
			}
			kgroups[minIndex].push_back(j);
		}
		// find newCenter
		for (int j = 0; j < kCluster; j++)
		{
			vector<float> distanceToRest(kgroups[j].size(), 0.0f);
			for (int ii = 0; ii < kgroups[j].size(); ii++)
			{
				for (int kk = 0; kk < kgroups[j].size(); kk++)
				{
					float d = 1.0f - (inputData[kgroups[j][ii]] DOT inputData[kgroups[j][kk]]);
					distanceToRest[ii] += d;
				}
			}
			float minDis = FLT_MAX;
			float minIndex = -1;
			for (int ii = 0; ii < distanceToRest.size(); ii++)
			{
				if (distanceToRest[ii] > minDis) continue;
				minDis = distanceToRest[ii];
				minIndex = kgroups[j][ii];
			}
			kCenters[j] = minIndex;
		}
	}
	vector<vec> res;
	for (int i = 0; i < kCenters.size(); i++) res.push_back(inputData[kCenters[i]]);
	return res;
}

vector<vec> kmedoidsClustering(vector<vec> inputData, int kCluster = 3)
{
	vector<int> kCenters;
	// initial center
	int index = rand() % inputData.size();
	kCenters.push_back(index);
	float minDis = 0.0f;
	int index1 = -1;
	for (int ii = 0; ii < inputData.size(); ii++){
		if (ii != index && dist(inputData[ii], inputData[index]) > minDis){
			index1 = ii;
			minDis = dist(inputData[ii], inputData[index]);
		}
	}
	kCenters.push_back(index1);
	minDis = 0.0;
	int index2 = -1;
	for (int ii = 0; ii < inputData.size(); ii++){
		if (ii != index  && ii != index1 && dist(inputData[ii], inputData[index]) + dist(inputData[ii], inputData[index1]) > minDis){
			index2 = ii;
			minDis = dist(inputData[ii], inputData[index]) + dist(inputData[ii], inputData[index1]);
		}
	}
	kCenters.push_back(index2);
	// clustering
	for (int i = 0; i < 100; i++)
	{
		// check group
		vector<vector<int>> kgroups(kCluster);
		for (int j = 0; j < inputData.size(); j++)
		{
			float d0 = 1.0f - (inputData[j] DOT inputData[kCenters[0]]);
			float d1 = 1.0f - (inputData[j] DOT inputData[kCenters[1]]);
			float d2 = 1.0f - (inputData[j] DOT inputData[kCenters[2]]);
			if (d0 <= d1 && d0 <= d2) kgroups[0].push_back(j);
			if (d1 <= d0 && d1 <= d2) kgroups[1].push_back(j);
			if (d2 <= d0 && d2 <= d1) kgroups[2].push_back(j);
		}
		// find newCenter
		for (int j = 0; j < kCluster; j++)
		{
			vector<float> distanceToRest(kgroups[j].size(), 0.0f);
			for (int ii = 0; ii < kgroups[j].size(); ii++)
			{
				for (int kk = 0; kk < kgroups[j].size(); kk++)
				{
					float d = 1.0f - (inputData[kgroups[j][ii]] DOT inputData[kgroups[j][kk]]);
					distanceToRest[ii] += d;
				}
			}
			float minDis = FLT_MAX;
			float minIndex = -1;
			for (int ii = 0; ii < distanceToRest.size(); ii++)
			{
				if (distanceToRest[ii] > minDis) continue;
				minDis = distanceToRest[ii];
				minIndex = kgroups[j][ii];
			}
			kCenters[j] = minIndex;
		}
	}
	vector<vec> res;
	res.push_back(inputData[kCenters[0]]);
	res.push_back(inputData[kCenters[1]]);
	res.push_back(inputData[kCenters[2]]);
	return res;
}

static bool buildKdTree(TriMesh *mesh, TKDTree &tree, TKDTreeData &treeData) // kd tree of face triangles
{
	treeData.resize(mesh->faces.size());

	for (int faceID = 0; faceID<(int)mesh->faces.size(); faceID++) {
		ivec3 idx(mesh->faces[faceID][0], mesh->faces[faceID][1], mesh->faces[faceID][2]);
		G3D::Vector3 v0(mesh->vertices[idx[0]].data());
		G3D::Vector3 v1(mesh->vertices[idx[1]].data());
		G3D::Vector3 v2(mesh->vertices[idx[2]].data());
		treeData[faceID].set(TKDT::NamedTriangle(v0, v1, v2, faceID));
	}

	tree.init(treeData.begin(), treeData.end());

	return true;
}

vector<vec3> getViewPoint(vec3 center, TKDTree& tree, vector<vec3> viewPoint, float unitLength)
{
	vector<vec3> renderViews;
	int totalHit = 0;
	float eps = 1e-5;
	vector<vec> possibleView;
	for (int ii = 0; ii < viewPoint.size(); ii++)
	{
		vec3 rayDirP = viewPoint[ii];
		vec3 rayOriginP = center + rayDirP * eps; // add eps for offset
		Thea::Ray3 rayP(G3D::Vector3(rayOriginP.data()), G3D::Vector3(rayDirP.data()));
		if (tree.rayIntersectionTime(rayP) < -0.5){
			totalHit++;
			possibleView.push_back(viewPoint[ii]);
		}
	}
	//cout << totalHit << endl;
	int needViewDirection = 4;
	if (possibleView.size() < needViewDirection) return renderViews;
	vector<vec> finalDir = kmedoidsClusteringNew(possibleView, needViewDirection); // kmedoidsClustering(possibleView);
	vector<float> disScale = { 0.25f, 0.5f, 0.75f };
	//vector<float> disScale = { 0.5f, 0.75f, 1.0f };
	vector<vec> possibleUp = { vec(1, 0, 0), vec(0, 1, 0), vec(0, 0, 1) };
	int viewCount = 0;
	for (int i = 0; i < finalDir.size(); i++)
	{
		int up0, up1;
		if (abs(finalDir[i][0]) >= abs(finalDir[i][1]) && abs(finalDir[i][0]) >= abs(finalDir[i][2]))
		{
			up0 = 1; up1 = 2;
		}
		if (abs(finalDir[i][1]) >= abs(finalDir[i][0]) && abs(finalDir[i][1]) >= abs(finalDir[i][2]))
		{
			up0 = 0; up1 = 2;
		}
		if (abs(finalDir[i][2]) >= abs(finalDir[i][0]) && abs(finalDir[i][2]) >= abs(finalDir[i][1]))
		{
			up0 = 0; up1 = 1;
		}
		for (int jj = 0; jj < disScale.size(); jj++)
		{
			vec currentView = center + finalDir[i] * disScale[jj] * unitLength;
			renderViews.push_back(center);
			renderViews.push_back(currentView);
			renderViews.push_back(possibleUp[up0]);
			renderViews.push_back(center);
			renderViews.push_back(currentView);
			renderViews.push_back(-possibleUp[up0]);
			renderViews.push_back(center);
			renderViews.push_back(currentView);
			renderViews.push_back(possibleUp[up1]);
			renderViews.push_back(center);
			renderViews.push_back(currentView);
			renderViews.push_back(-possibleUp[up1]);
		}
	}
	return renderViews;
}



int main_pair(string name1, string name2, string name3)
{
	

	string str1("Models/Airplane/" + name1 + ".off");
	string str2("Models/Airplane/" + name2 + ".off");
	string str3("viewPosition.txt");
	string str4("Corr/Airplane/" + name3 + ".pts");
	string outPath = "Save/Airplane/" + name3 + "/";
	_mkdir(outPath.c_str());
	string str5(outPath + name1 + ".txt");
	string str6(outPath + name2 + ".txt");


	TriMesh *mesh0 = TriMesh::read(str1);
	mesh0->bsphere.valid = false; mesh0->need_bsphere();
	float fov0 = 43.0 / 180.0f;
	float unitLength0 = mesh0->bsphere.r / tan(fov0 / 2 * 3.14159);
	cout << "unitLength " << unitLength0 << endl;
	//mesh->normals.clear();
	mesh0->need_normals();		// this compute face areas as well!
	cout << "begin buildKdTree " << endl;
	TKDTree tree0;
	TKDTreeData treeData0;
	buildKdTree(mesh0, tree0, treeData0);

	TriMesh *mesh1 = TriMesh::read(str2);
	mesh1->bsphere.valid = false; mesh1->need_bsphere();
	float fov1 = 43.0 / 180.0f;
	float unitLength1 = mesh1->bsphere.r / tan(fov1 / 2 * 3.14159);
	cout << "unitLength " << unitLength1 << endl;
	//mesh->normals.clear();
	mesh1->need_normals();		// this compute face areas as well!
	cout << "begin buildKdTree " << endl;
	TKDTree tree1;
	TKDTreeData treeData1;
	buildKdTree(mesh1, tree1, treeData1);
	ifstream fv(str3);
	vector<vec> viewPoint;
	float x, y, z;
	while (fv >> x >> y >> z){
		vec p(x, y, z);
		viewPoint.push_back(p);
	}
	cout << viewPoint.size() << endl;
	vector<vec3> mesh0Sample;
	vector<vec3> mesh1Sample;
	ifstream fin(str4);
	float x0, y0, z0, x1, y1, z1;
	while (fin >> x0 >> y0 >> z0 >> x1 >> y1 >> z1)
	{
		vec3 p0(x0, y0, z0);
		vec3 p1(x1, y1, z1);
		mesh0Sample.push_back(p0);
		mesh1Sample.push_back(p1);
	}
	cout << "total Samples " << mesh0Sample.size() << " " << mesh1Sample.size() << endl;
	// generate positive pairs
	int needPositive = 0;
	srand(time(0));
	ofstream fout0(str5);
	ofstream fout1(str6);
	int needViewDirection = 3;
	while (needPositive < 10)
	{
		int currentPair = rand() % mesh0Sample.size();
		vector<vec3> view0 = getViewPoint(mesh0Sample[currentPair], tree0, viewPoint, unitLength0);
		vector<vec3> view1 = getViewPoint(mesh1Sample[currentPair], tree1, viewPoint, unitLength1);
		if (view0.size() > 0 && view1.size() > 0)
		{
			for (int i = 0; i < view0.size() / 3; i++)
			{
				fout0 << view0[i * 3][0] << " " << view0[i * 3][1] << " " << view0[i * 3][2] <<" ";
				fout0 << view0[i * 3 + 1][0] << " " << view0[i * 3 + 1][1] << " " << view0[i * 3 + 1][2] << " ";
				fout0 << view0[i * 3 + 2][0] << " " << view0[i * 3 + 2][1] << " " << view0[i * 3 + 2][2] << " ";
				fout0 << "possitive_" << to_string(needPositive) << "_" << to_string(i) << ".png" << endl;
				fout1 << view1[i * 3][0] << " " << view1[i * 3][1] << " " << view1[i * 3][2] << " ";
				fout1 << view1[i * 3 + 1][0] << " " << view1[i * 3 + 1][1] << " " << view1[i * 3 + 1][2] << " ";
				fout1 << view1[i * 3 + 2][0] << " " << view1[i * 3 + 2][1] << " " << view1[i * 3 + 2][2] << " ";
				fout1 << "possitive_" << to_string(needPositive) << "_" << to_string(i + view1.size() / 3) << ".png" << endl;
			}
			needPositive++;
		}
	}
	int needNagative = 0;
	while (needNagative < 10)
	{
		int currentPair0 = rand() % mesh0Sample.size();
		int currentPair1 = rand() % mesh0Sample.size();
		if (currentPair0 == currentPair1) continue;
		vector<vec3> view0 = getViewPoint(mesh0Sample[currentPair0], tree0, viewPoint, unitLength0);
		vector<vec3> view1 = getViewPoint(mesh1Sample[currentPair1], tree1, viewPoint, unitLength1);
		if (view0.size() > 0 && view1.size() > 0)
		{
			for (int i = 0; i < view0.size() / 3; i++)
			{
				fout0 << view0[i * 3][0] << " " << view0[i * 3][1] << " " << view0[i * 3][2] << " ";
				fout0 << view0[i * 3 + 1][0] << " " << view0[i * 3 + 1][1] << " " << view0[i * 3 + 1][2] << " ";
				fout0 << view0[i * 3 + 2][0] << " " << view0[i * 3 + 2][1] << " " << view0[i * 3 + 2][2] << " ";
				fout0 << "negative_" << to_string(needNagative) << "_" << to_string(i) << ".png" << endl;
				fout1 << view1[i * 3][0] << " " << view1[i * 3][1] << " " << view1[i * 3][2] << " ";
				fout1 << view1[i * 3 + 1][0] << " " << view1[i * 3 + 1][1] << " " << view1[i * 3 + 1][2] << " ";
				fout1 << view1[i * 3 + 2][0] << " " << view1[i * 3 + 2][1] << " " << view1[i * 3 + 2][2] << " ";
				fout1 << "negative_" << to_string(needNagative) << "_" << to_string(i + view1.size() / 3) << ".png" << endl;
			}
			needNagative++;
		}
	}
	return 0;
}





int main_test(string name)
{
	//The path of mesh model
	const string str1 = "Models/Airplane/"+ name + ".off";
	//The path of point and index
	const string str2 = "Generate/" + name + ".txt";
	//The path of camera_view
	const string str3 = "viewPosition.txt";
	//The path of fout
	const string str4 = "Save/Render_view/" + name + ".txt";
	const string str5 = name;
	const string str6 = "Save/Sample.txt";


	//读取mesh模型
	TriMesh *mesh = TriMesh::read(str1);
	ifstream fin(str2);
	vector<int> validGT;
	vector<vec> gtPoint;
	float x, y, z;
	int index;
	//validGT 存放index的队列
	//gtPoint 存放point的队列
	while (fin >> index){
		if (index != -1){
			validGT.push_back(index);
			fin >> x >> y >> z;
			vec p(x, y, z);
			gtPoint.push_back(p);
		}
		else{
			validGT.push_back(-1);
			vec p(100, 100, 100);
			gtPoint.push_back(p);
		}
	}
	cout << gtPoint.size() << endl;
	ifstream fv(str3);
	vector<vec> viewPoint;

	//第三个string对应路径的viewPoint的点 代表所看的方向
	while (fv >> x >> y >> z){
		vec p(x, y, z);
		viewPoint.push_back(p);
	}
	cout << viewPoint.size() << endl;

	//mesh的未知读取
	mesh->bsphere.valid = false; 
	mesh->need_bsphere();
	//cout << mesh->bsphere.r  * tan(0.125 * 3.14159)<< endl;
	float fov = 43.0 / 180.0f;
	float unitLength = mesh->bsphere.r / tan(fov / 2 * 3.14159);
	cout << "unitLength " << unitLength << endl;
	//mesh->normals.clear();
	mesh->need_normals();		// this compute face areas as well!
	cout << "begin buildKdTree " << endl;
	TKDTree tree;
	TKDTreeData treeData;
	buildKdTree(mesh, tree, treeData);
	cout << "end buildKdTree " << endl;


	ofstream fout(str4);
	int currentID = 0;
	ofstream fsample(str6);
	int totalCound = 0;
	for (int j = 0; j < gtPoint.size(); j++)
	{
		if (validGT[j] < 0) continue;
		int totalHit = 0;
		
		
		float eps = 1e-5;
		vector<vec> possibleView;
		for (int ii = 0; ii < viewPoint.size(); ii++)
		{
			vec3 rayDirP = viewPoint[ii];
			vec3 rayOriginP = gtPoint[j] + rayDirP * eps; // add eps for offset
			Thea::Ray3 rayP(G3D::Vector3(rayOriginP.data()), G3D::Vector3(rayDirP.data()));
			if (tree.rayIntersectionTime(rayP) < -0.5){
				totalHit++;
				possibleView.push_back(viewPoint[ii]);
			}
		}
		cout << "totalHit: " << totalHit << endl;
		if (possibleView.size() < 4) 
			continue;
		fsample << gtPoint[j][0] << " " << gtPoint[j][1] << " " << gtPoint[j][2] << endl;
		vector<vec> finalDir = kmedoidsClusteringNew(possibleView, 3);
		vector<float> disScale = { 0.25f, 0.5f, 0.75f };
		//vector<float> disScale = { 0.125f, 0.25f, 0.5f };
		vector<vec> possibleUp = { vec(1, 0, 0), vec(0, 1, 0), vec(0, 0, 1)};
		int viewCount = 0;
		for (int i = 0; i < finalDir.size(); i++)
		{
			int up0, up1;
			if (abs(finalDir[i][0]) >= abs(finalDir[i][1]) && abs(finalDir[i][0]) >= abs(finalDir[i][2]))
			{
				up0 = 1; up1 = 2;
			}
			if (abs(finalDir[i][1]) >= abs(finalDir[i][0]) && abs(finalDir[i][1]) >= abs(finalDir[i][2]))
			{
				up0 = 0; up1 = 2;
			}
			if (abs(finalDir[i][2]) >= abs(finalDir[i][0]) && abs(finalDir[i][2]) >= abs(finalDir[i][1]))
			{
				up0 = 0; up1 = 1;
			}
			for (int jj = 0; jj < disScale.size(); jj++)
			{
				vec currentView = gtPoint[j] + finalDir[i] * disScale[jj] * unitLength;
				fout << gtPoint[j][0] << " " << gtPoint[j][1] << " " << gtPoint[j][2];
				fout << " " << currentView[0] << " " << currentView[1] << " " << currentView[2];
				fout << " " << possibleUp[up0][0] << " " << possibleUp[up0][1] << " " << possibleUp[up0][2];
				fout << " " << str5 << "_" << to_string(currentID /*validGT[j]*/) << "_" << to_string(viewCount) << ".png" << endl;
				viewCount++;
				fout << gtPoint[j][0] << " " << gtPoint[j][1] << " " << gtPoint[j][2];
				fout << " " << currentView[0] << " " << currentView[1] << " " << currentView[2];
				fout << " " << -possibleUp[up0][0] << " " << -possibleUp[up0][1] << " " << -possibleUp[up0][2];
				fout << " " << str5 << "_" << to_string(currentID /*validGT[j]*/) << "_" << to_string(viewCount) << ".png" << endl;
				viewCount++;
				fout << gtPoint[j][0] << " " << gtPoint[j][1] << " " << gtPoint[j][2];
				fout << " " << currentView[0] << " " << currentView[1] << " " << currentView[2];
				fout << " " << possibleUp[up1][0] << " " << possibleUp[up1][1] << " " << possibleUp[up1][2];
				fout << " " << str5 << "_" << to_string(currentID /*validGT[j]*/) << "_" << to_string(viewCount) << ".png" << endl;
				viewCount++;
				fout << gtPoint[j][0] << " " << gtPoint[j][1] << " " << gtPoint[j][2];
				fout << " " << currentView[0] << " " << currentView[1] << " " << currentView[2];
				fout << " " << -possibleUp[up1][0] << " " << -possibleUp[up1][1] << " " << -possibleUp[up1][2];
				fout << " " << str5 << "_" << to_string(currentID /*validGT[j]*/) << "_" << to_string(viewCount) << ".png" << endl;
				viewCount++;
			}
		}
		currentID++;
	}

	return 0;
}

int main()
{
	/*string path("D:/Documents/Desktop/Junior/2019/CG/renderScript/Airplane.txt");
	ifstream file(path);
	string str_temp;
	while (getline(file, str_temp))
	{
		
		main_test(str_temp);
	}*/
	string path("D:/Documents/Desktop/Junior/2019/CG/renderScript/Airplane_corr.txt");
	ifstream file(path);
	string str_temp;
	
	string name1, name2, name3;
	while (file >> name1 >> name2 >> name3)
	{
		main_pair(name1, name2, name3);
	}
}