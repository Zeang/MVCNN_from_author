
// Standard includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <omp.h>
#include <sys/stat.h>
#include <ctime>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Mesh.h"
#include "Camera.h"
#include "Viewport.h"

// Sampling includes
#include"SamplePoissonDisk.h"

// OpenCV
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

//filesystem
#include <direct.h> 

using namespace std;
using namespace trimesh;

vector<Viewport> views;
vector<string> renderList;

// Global variables
GLuint width = 227, height = 227;


// Tool functions
void snapshot(string saveName);
void saveDepth(string saveName);
inline bool exists (const std::string& name);
glm::vec3 getVector(trimesh::vec3 vec);

int main_test(string name1, string name2);
int main()
{
	string path("D:/Documents/Desktop/Junior/2019/CG/renderScript/Airplane_corr.txt");
	ifstream file(path);
	string str_temp;
	int number = 139;
	while (getline(file, str_temp))
	{
		if (number > 0)
		{
			number--;
			continue;
		}
			
		int index1 = str_temp.find_first_of(" ");
		int index2 = str_temp.find_last_of(" ");
		string name1 = str_temp.substr(0, index1);
		string name2 = str_temp.substr(index1 + 1, index2 - index1 - 1);
		string name3 = str_temp.substr(index2 + 1);

		main_test(name1, name3);
		main_test(name2, name3);
	}
}


int main_test(string name1, string name2)
{
	vector<glm::vec3> cameraPos, targetPos, upVector;

	const string model_class = "Airplane/";
	const string model_path = "Models/" + model_class;
	const string save_model = "pic_corr_mode/" + name2;

	const string str1 = name1 + ".off";
	const string str2 = "../../generateRenderView/generateRenderView/Save/"+ model_class + name2  + "/" + name1 + ".txt";
	const string str3 = "pic_Save/" + model_class + name2;
	const string str4 = save_model + "/Airplane.txt";
	
	/*const string name = "1021a0914a7207aff927ed529ad90a11";
	const string model_class = "Airplane/";
	const string model_path = "Models/" + model_class;
	const string save_model = "pic_Save_mesh/" + model_class + name + "/";

	const string str1 = name + ".off";
	const string str2 = "../../generateRenderView/generateRenderView/Save/Render_view/" + name + ".txt";
	const string str3 = "pic_Save_mesh/" + model_class + name;*/

	// Read mesh
	//TriMesh *mesh = TriMesh::read(argv[1]);
	_mkdir(save_model.c_str());
	_mkdir(("pic_Save_mesh/" + model_class).c_str());
	_mkdir(str3.c_str());
	TriMesh *mesh = TriMesh::read(model_path + str1);
	mesh->normals.clear();
	mesh->need_normals();

	//// Get samples
	/*SamplePoissonDisk sampleMesh(mesh);
	vector<Sample> denseSample = sampleMesh.GetSample(stoi(str2), false);*/
	//vector<Sample> denseSample = sampleMesh.GetSample(stoi(argv[2]), false);
	mesh->need_bbox();
	//ifstream fin(argv[2]);
	//vector<glm::vec3> camPos;
	//vector<int> valid;
	//float x,y,z;
	//while(fin >> x >> y >> z)
	//{
	//	glm::vec3 p(x * 1.5f,y* 1.5f,z* 1.5f);
	//	camPos.push_back(p);
	//}


	//// Get labels
	//ifstream fin(argv[3]);
	//vector<int> faceLabel;
	//int label;
	//while (fin >> label)
	//	faceLabel.push_back(label);

	//// Output
	//ofstream outSample(argv[4]);
	//ofstream outSamplelable(argv[5]);
	//float meshRadious = dist(mesh->bbox.max, mesh->bbox.min);
	//for (size_t j = 0; j < denseSample.size(); j++)
	//{
	//	trimesh::vec3 ccenterPos = denseSample[j].position;
	//	trimesh::vec3 ccenterNor = denseSample[j].normal;
	//	ccenterNor = normalize(ccenterNor);

	//	// Output sampling points info
	//	outSample	<< ccenterPos[0] << " " << ccenterPos[1] << " " << ccenterPos[2] << " " 
	//		<< ccenterNor[0] << " " << ccenterNor[1] << " " << ccenterNor[2] << " "
	//		<< denseSample[j].face_id << endl;

	//	// Save sampling info: target pos & view pos
	//	for (int i = 0; i < 4; i++) 
	//	{
	//		targetPos.push_back(getVector(ccenterPos));
	//		//cameraPos.push_back( getVector( ccenterPos + ccenterNor * (meshRadious / 20.f * (float)(pow(2, i + 1) + 2)) ) );
	//		cameraPos.push_back( getVector( ccenterPos + ccenterNor * (meshRadious / 20.f * (float)(i + 4)) ) );
	//	}

	//	// Output labels for training
	//	outSamplelable << j << " " << faceLabel[denseSample[j].face_id] + 1 << endl;
	//}

	/* Rendering */

	omp_set_dynamic(0);     // Explicitly disable dynamic teams
	omp_set_num_threads(6); // Use 4 threads for all consecutive parallel regions
	
	//loading centerPos
	ifstream fin(str2);
	vector<string> outNames;
	float tx, ty, tz, cx,cy,cz,ux,uy,uz;
	string outName;
	vector<int> valid;
	int j = 0;
	int Point_number = 2 * 36;
	while(fin >> tx >> ty >> tz >> cx >> cy >> cz >> ux >> uy >> uz >> outName)
	{

		valid.push_back(j++);
		glm::vec3 t(tx,ty,tz);
		glm::vec3 c(cx,cy,cz);
		glm::vec3 u(ux,uy,uz);
		cameraPos.push_back(c);
		targetPos.push_back(t);
		upVector.push_back(u);
		string fileName(str3);
		//cout << fileName +  << endl;
		outNames.push_back(fileName + "//" + outName);
	}
	//ifstream fin(argv[2]);
	//int index;
	//int count = 0;
	//float x,y,z;
	//while(fin >> index){
	//	if(index != -1){
	//		valid.push_back(index);
	//		fin >> x >> y >> z;
	//		glm::vec3 p(x,y,z);
	//		glm::vec3 n(0,0,1);
	//		glm::vec3 n1(0,-1,0);
	//		glm::vec3 n2(-1,0,0);
	//		targetPos.push_back(p);
	//		cameraPos.push_back(p + 1.25f * n);
	//		targetPos.push_back(p);
	//		cameraPos.push_back(p + 2.5f * n);
	//		targetPos.push_back(p);
	//		cameraPos.push_back(p + 1.25f * n1);
	//		targetPos.push_back(p);
	//		cameraPos.push_back(p + 2.5f * n1);
	//	}
	//	else{
	//		valid.push_back(-1);
	//		glm::vec3 p(100,100,100);
	//		glm::vec3 n(0,0,1);
	//		targetPos.push_back(p);
	//		cameraPos.push_back(p + 2.0f * n);
	//		targetPos.push_back(p);
	//		cameraPos.push_back(p + 3.0f * n);
	//		targetPos.push_back(p);
	//		cameraPos.push_back(p + 4.0f * n);
	//		targetPos.push_back(p);
	//		cameraPos.push_back(p + 5.0f * n);
	//	}
	//}
	cout << "end load points" << endl;
	//float x,y,z,nx,ny,nz;
	//while(fin >> x >> y >> z >> nx >> ny >> nz){
	//	glm::vec3 p(x,y,z);
	//	glm::vec3 n(nx,ny,nz);
	//	targetPos.push_back(p);
	//	cameraPos.push_back(p + 0.75f * n);
	//	targetPos.push_back(p);
	//	cameraPos.push_back(p + 1.0f * n);
	//	targetPos.push_back(p);
	//	cameraPos.push_back(p + 1.5f * n);
	//	targetPos.push_back(p);
	//	cameraPos.push_back(p + 2.0f * n);
	//}

	//原来的视角序列
	//vector<int> valid;
	////ofstream outSample(argv[4]);
	//ofstream outSample(str4);
	//for (size_t j = 0; j < denseSample.size(); j++){
	//	valid.push_back(j);
	//	trimesh::vec3 ccenterPos = denseSample[j].position;
	//	outSample	<< ccenterPos[0] << " " << ccenterPos[1] << " " << ccenterPos[2] << " "<< endl;
	//	glm::vec3 p(ccenterPos[0],ccenterPos[1],ccenterPos[2]);
	////		glm::vec3 p(x,y,z);
	//		glm::vec3 n(0,0,1);
	//		glm::vec3 n1(0,-1,0);
	//		glm::vec3 n2(-1,0,0);
	//		targetPos.push_back(p);
	//		cameraPos.push_back(p + 1.25f * n);
	//		targetPos.push_back(p);
	//		cameraPos.push_back(p + 2.5f * n);
	//		targetPos.push_back(p);
	//		cameraPos.push_back(p + 1.25f * n1);
	//		targetPos.push_back(p);
	//		cameraPos.push_back(p + 2.5f * n1);
	//}

	// Init GLFW
	glfwInit();

	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	//#pragma omp parallel for


	GLFWwindow* window = glfwCreateWindow(width, height, "View", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;

	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, width, height);

	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	// Initilize viewports
	string modelName(model_path + str1);
	//string modelName(argv[1]);
	Viewport view(glm::vec4(0, 0, width, height), modelName.c_str());
	for(int i = 0; i < targetPos.size(); i++)
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//cout << i / 36 << " " <<i % 36 <<" "<< targetPos[i][0] - cameraPos[i][0] << " "<< targetPos[i][1] - cameraPos[i][1] << " "<< targetPos[i][2] - cameraPos[i][2] <<endl; //glm::distance(targetPos[i], cameraPos[i]) << endl;
		view.updateCamera(targetPos[i], cameraPos[i]);
		view.updateUP(upVector[i]);
		view.updateFrame(GL_FILL);
		//cout << outNames[i] << endl;
		snapshot(outNames[i]);
	}

	// render patches
	//最初的render
	//for(int i = 0; i < valid.size(); i++)
	//{
	//	//string folder(argv[3]);
	//	//string saveName(argv[1]);
	//	string folder(str3);
	//	string saveName(str1);
	//	string saveName0 =  folder + "/" + saveName.substr(0, saveName.length() - 4) + "_" + to_string(valid[i])  +"_"+ to_string(0)+".png";
	//	if(!exists(saveName) && saveName.length() > 3)
	//	{
	//		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//		view.updateCamera(targetPos[i * 4 ], cameraPos[i * 4]);
	//		view.updateUP(glm::vec3(0,1,0));
	//		view.updateFrame(GL_FILL);
	//		snapshot(saveName0);
	//	}
	//	string saveName1 =  folder + "/" + saveName.substr(0, saveName.length() - 4) + "_" + to_string(valid[i])  +"_"+ to_string(1)+".png";
	//	if(!exists(saveName) && saveName.length() > 3)
	//	{
	//		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//		view.updateCamera(targetPos[i * 4 + 1], cameraPos[i * 4 + 1]);
	//		view.updateUP(glm::vec3(0,1,0));
	//		view.updateFrame(GL_FILL);
	//		snapshot(saveName1);
	//	}
	//	string saveName2 =  folder + "/" + saveName.substr(0, saveName.length() - 4) + "_" + to_string(valid[i])  +"_"+ to_string(2)+".png";
	//	if(!exists(saveName) && saveName.length() > 3)
	//	{
	//		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//		view.updateCamera(targetPos[i * 4 + 2], cameraPos[i * 4 + 2]);
	//		view.updateUP(glm::vec3(0,0,1));
	//		view.updateFrame(GL_FILL);
	//		snapshot(saveName2);
	//	}
	//	string saveName3 =  folder + "/" + saveName.substr(0, saveName.length() - 4) + "_" + to_string(valid[i])  +"_"+ to_string(3)+".png";
	//	if(!exists(saveName) && saveName.length() > 3)
	//	{
	//		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//		view.updateCamera(targetPos[i * 4 + 3], cameraPos[i * 4 + 3]);
	//		view.updateUP(glm::vec3(0,0,1));
	//		view.updateFrame(GL_FILL);
	//		snapshot(saveName3);
	//	}
	//}

	/*for(int i = 0; i < targetPos.size(); i++)
	{
		if(valid[i/4] != -1)
		{
			string folder(argv[3]);
			string saveName(argv[1]);
			string saveName0 =  folder + "//" + saveName.substr(0, saveName.length() - 4) + "_" + to_string(i / 4) + "_" + to_string(i % 4) +"_"+ to_string(0)+".png";
			if(!exists(saveName) && saveName.length() > 3)
			{
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				view.updateCamera(targetPos[i], cameraPos[i]);
				view.updateUP(glm::vec3(0,0,1));
				view.updateFrame(GL_FILL);
				snapshot(saveName0);
			}
			string saveName1 =  folder + "//" + saveName.substr(0, saveName.length() - 4) + "_" + to_string(i / 4) + "_" + to_string(i % 4) +"_"+ to_string(1)+".png";
			if(!exists(saveName) && saveName.length() > 3)
			{
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				view.updateCamera(targetPos[i], cameraPos[i]);
				view.updateUP(glm::vec3(0,0,-1));
				view.updateFrame(GL_FILL);
				snapshot(saveName1);
			}
			string saveName2 =  folder + "//" + saveName.substr(0, saveName.length() - 4) + "_" + to_string(i / 4) + "_" + to_string(i % 4) +"_"+ to_string(2)+".png";
			if(!exists(saveName) && saveName.length() > 3)
			{
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				view.updateCamera(targetPos[i], cameraPos[i]);
				view.updateUP(glm::vec3(1,0,0));
				view.updateFrame(GL_FILL);
				snapshot(saveName2);
			}
			string saveName3 =  folder + "//" + saveName.substr(0, saveName.length() - 4) + "_" + to_string(i / 4) + "_" + to_string(i % 4) +"_"+ to_string(3)+".png";
			if(!exists(saveName) && saveName.length() > 3)
			{
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				view.updateCamera(targetPos[i], cameraPos[i]);
				view.updateUP(glm::vec3(-1,0,0));
				view.updateFrame(GL_FILL);
				snapshot(saveName3);
			}
		}
	}*/

	//for(int i = 0; i < camPos.size(); i++)
	//{
	//	string folder(argv[3]);
	//	string saveName(argv[1]);
	//	saveName =  folder + "//" + saveName.substr(0, saveName.length() - 4) + "_Cam_" + to_string(i) + ".png";
	//	if(!exists(saveName) && saveName.length() > 3)
	//	{
	//		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//		view.updateCamera(glm::vec3(0.0f,0.0f,0.0f), camPos[i]);
	//		view.updateFrame(GL_FILL);
	//		//snapshot(saveName);
	//		saveDepth(saveName);
	//	}
	//}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	return 0;
}



void snapshot(string saveName)
{
	cv::Mat img(height, width, CV_8UC3);
	glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
	glPixelStorei(GL_PACK_ROW_LENGTH, img.step / img.elemSize());
	glReadPixels(0, 0, img.cols, img.rows, GL_BGR_EXT, GL_UNSIGNED_BYTE, img.data);
	cv::Mat flipped(img);
	cv::flip(img, flipped, 0);
	//saveName = saveName.append(".png");
	cv::imwrite(saveName, img);
}

void saveDepth(string saveName)
{
	float zNear = 1.0f;
	float zFar = 2.0f;
	vector< GLfloat > depth(width * height, 0);
	glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, &depth[0]); // read depth buffer
	cv::Mat img(height, width, CV_32F);// output depth image
	for (int i = 0; i < img.rows; i++ )
	{
		for (int j = 0; j < img.cols; j++)
		{
			depth[i*img.cols + j] = (2.0 * zNear * zFar) / (zFar + zNear - (2.0f * depth[i*img.cols + j] - 1 ) * (zFar - zNear));
			depth[i*img.cols + j] = (depth[i*img.cols + j] - zNear) / (zFar - zNear);
			//cout << depth[i*img.cols + j] << endl;
			//img.at<float>(i, j) = (int)((1.0f - depth[i*img.cols + j] /6) * 255); // flip image
			img.at<float>(i, j) = (1.0f - depth[i*img.cols + j]) * 255;
			//fout << depth[i*img.cols + j] << " ";
		}
		//fout << endl;
	}

	cv::Mat flipped(img);
	cv::flip(img, flipped, 1);

	cv::Mat imgRGB(height, width, CV_32FC3);// output depth image
	for (int i = 0; i < imgRGB.rows; i++)
	{
		for (int j = 0; j < imgRGB.cols; j++)
		{
			imgRGB.at<cv::Vec3f>(i, j) = cv::Vec3f(img.at<float>(i, j), img.at<float>(i, j), img.at<float>(i, j)); // flip image
			//cout << imgRGB.at<cv::Vec3f>(i, j) << " " << img.at<float>(i, j) << endl;
		}
	}

	cv::imwrite(saveName, imgRGB);
}

inline bool exists (const std::string& name) 
{
	return false;
	struct stat buffer;   
	return (stat (name.c_str(), &buffer) == 0); 
}

glm::vec3 getVector(trimesh::vec3 vec)
{
	return glm::vec3(vec[0], vec[1], vec[2]);
}