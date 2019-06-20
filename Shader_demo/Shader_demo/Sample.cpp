#include "Sample.h"


Sample::Sample(void)
{
	vec3 p(0,0,0);
	vec3 n(0,0,1);
	position = p;
	normal = n;
	weight = 0;
	face_id = -1;
	patch_id = -1;
}


Sample::~Sample(void)
{

}


Sample::Sample(vec3 pos, vec3 nor, float weg, int id )
{
	position = pos;
	normal = nor;
	weight = weg;
	face_id = id;
}



