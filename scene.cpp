
#include "CVector.h"
#include "string.h"
#include "scene.h"
#include "raytracer.h"

namespace Raytracer {


void Primitive::SetName( char* a_Name )
{
	delete m_Name; 
	m_Name = new char[strlen( a_Name ) + 1]; 
	strcpy( m_Name, a_Name ); 
}


Material::Material() :
	m_Color( Color( 0.2f, 0.2f, 0.2f ) ),
	m_Refl( 0 ),m_Refr( 0 ),m_RefrIndex( 1.0 ), m_Diff( 0.2f )
{

}


int Sphere::Intersect( Ray& a_Ray, float& a_Dist )
{
	vector3 v = a_Ray.GetOrigin() - m_Centre;
	float b = -DOT( v, a_Ray.GetDirection() );
	float det = (b * b) - DOT( v, v ) + m_SqRadius;
	int retval = MISS;
	if (det > 0)
	{
		det = sqrtf( det );
		float i1 = b - det;
		float i2 = b + det;
		if (i2 > 0)//b and set are opposite
		{
			if (i1 < 0) //source is inside sphere
			{
				if (i2 < a_Dist) 
				{
					a_Dist = i2;
					retval = INPRIM;
				}
			}
			else
			{
				if (i1 < a_Dist)
				{
					a_Dist = i1;
					retval = HIT;
				}
			}
		}
	}
	return retval;
}


int PlanePrim::Intersect( Ray& a_Ray, float& a_Dist )
{
	float d = DOT( N, a_Ray.GetDirection() );
	if (d != 0)
	{
		float dist = -(DOT( N, a_Ray.GetOrigin() ) + D) / d;
		if (dist > 0)
		{
			if (dist < a_Dist) 
			{
				a_Dist = dist;
				return HIT;
			}
		}
	}
	return MISS;
}

vector3 PlanePrim::GetNormal( vector3& a_Pos )
{
	return N;
}


Scene::~Scene()
{
	delete m_Primitive;
}

void Scene::InitScene()
{
	m_Primitive = new Primitive*[100];
	// ground plane
	
	m_Primitive[0] = new CheckPlanePrim( vector3( 0, 1, 0 ),vector3( 1, 0, 0 ), 4.4f );
	

	// big sphere with large Z distance
	m_Primitive[1] = new Sphere( vector3( -2, 1.2f, 7 ), 2.3f );
	m_Primitive[1]->SetName( "big sphere" );
	m_Primitive[1]->GetMaterial()->SetReflection( 0.6f );
	m_Primitive[1]->GetMaterial()->SetRefraction( 0.8f );
	m_Primitive[1]->GetMaterial()->SetColor( Color( 0.5f, 0.3f, 0.9f ) );
	// small sphere with small Z distance
	m_Primitive[2] = new Sphere( vector3( -5.5f, -0.5, 7 ), 2 );
	m_Primitive[2]->SetName( "small sphere" );
	m_Primitive[2]->GetMaterial()->SetReflection( 0.7f );
	m_Primitive[2]->GetMaterial()->SetDiffuse( 0.1f );
	m_Primitive[2]->GetMaterial()->SetColor( Color( 0.32f, 0.91f, 0.2f ) );


	// light source 1
	/*m_Primitive[3] = new Sphere( vector3(5, 2, -3 ), 0.1f );
	m_Primitive[3]->Light( true );
	m_Primitive[3]->GetMaterial()->SetColor( Color( 0.9f, 0.5f, 1.0f ) );
	*/// light source 2
	m_Primitive[3] = new Sphere( vector3( 0, 2, -10 ), 0.1f );
	m_Primitive[3]->Light( true );
	m_Primitive[3]->GetMaterial()->SetColor( Color( 1.0f, 1.0f, 1.0f ) );
	// light source 2
	/*
	m_Primitive[5] = new Sphere( vector3( -5, 3 , -5), 0.1f );
	m_Primitive[5]->Light( true );
	m_Primitive[5]->GetMaterial()->SetColor( Color( 0.1f, 0.2f, 1.0f ) );
	*/// set number of primitives
	m_Primitive[4] = new Sphere( vector3( -10, 4, -13 ), 0.6f );
	m_Primitive[4]->Light( true );
	m_Primitive[4]->GetMaterial()->SetColor( Color( 1.0f, 1.0f, 1.0f ) );
	


	m_Primitives = 5;
}

}; // namespace Raytracer
