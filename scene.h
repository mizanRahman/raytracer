
#ifndef I_SCENE_H
#define I_SCENE_H

#include "raytracer.h"


namespace Raytracer {

// Intersection method return values
#define HIT		 1		// Ray hit primitive
#define MISS	 0		// Ray missed primitive
#define INPRIM	-1		// Ray started inside primitive


class Material
{
public:
	Material();
	void SetColor( Color& a_Color ) { m_Color = a_Color; }
	Color GetColor() { return m_Color; }
	void SetDiffuse( float a_Diff ) { m_Diff = a_Diff; }
	void SetReflection( float a_Refl ) { m_Refl = a_Refl; }	
	void SetRefraction( float a_Refr ) { m_Refr = a_Refr; }
		
	void SetRefIndex( float a_RefrIndex ) { m_RefrIndex = a_RefrIndex; }
	float GetSpecular() { return 1.0f - m_Diff; }
	float GetDiffuse() { return m_Diff; }
	float GetReflection() { return m_Refl; }	
	float GetRefraction() { return m_Refr; }	
	float GetRefIndex( ) { return m_RefrIndex;}


private:
	Color m_Color;
	float m_Refl;
	float m_Refr;
	float m_RefrIndex;
	float m_Diff;
};


class Primitive
{
public:
	enum
	{
		SPHERE = 1,
		PLANE,
		CHECKPLANE,
		CHESSBOARD
	};
	Primitive() : m_Name( 0 ), m_Light( false ) {};
	virtual Material* GetMaterial() { return &m_Material; }
	void SetMaterial( Material& a_Mat ) { m_Material = a_Mat; }
	virtual int GetType() = 0;
	virtual int Intersect( Ray& a_Ray, float& a_Dist ) = 0;
	virtual vector3 GetNormal( vector3& a_Pos ) = 0;
	virtual Color GetColor( vector3& ) { return m_Material.GetColor(); }
	virtual void Light( bool a_Light ) { m_Light = a_Light; }
	bool IsLight() { return m_Light; }
	void SetName( char* a_Name );
	char* GetName() { return m_Name; }
protected:
	Material m_Material;
	char* m_Name;
	bool m_Light;
};


class Sphere : public Primitive
{
public:
	int GetType() { return SPHERE; }
	Sphere( vector3& a_Centre, float a_Radius ) : 
		m_Centre( a_Centre ), m_SqRadius( a_Radius * a_Radius ), 
		m_Radius( a_Radius ), m_RRadius( 1.0f / a_Radius ) {};
	vector3& GetCentre() { return m_Centre; }
	float GetSqRadius() { return m_SqRadius; }
	int Intersect( Ray& a_Ray, float& a_Dist );
	vector3 GetNormal( vector3& a_Pos ) { return (a_Pos - m_Centre) * m_RRadius; }
private:
	vector3 m_Centre;
	float m_SqRadius, m_Radius, m_RRadius;
};



class PlanePrim : public Primitive
{
public:
	int GetType() { return PLANE; }
	PlanePrim() : N( 0, 0, 0 ), D( 0 ) {};
	PlanePrim( vector3& a_Normal, float a_D ) : N( a_Normal ), D( a_D ) {};
	vector3& GetNormal() { return N; }
	float GetD() { return D; }
	int Intersect( Ray& a_Ray, float& a_Dist );
	vector3 GetNormal( vector3& a_Pos );
private:

	vector3 N;
	float D;
};


class CheckPlanePrim : public PlanePrim
{
public:
	CheckPlanePrim( vector3& a_Normal, vector3& a_X, float a_D ) :PlanePrim( a_Normal, a_D )  
	{
		X=a_X;
			
		w_Material.SetReflection( 0 );
		w_Material.SetDiffuse( 1.0f );
		w_Material.SetColor( Color( 1.0f, 1.0f, 1.0f ) );
		
		b_Material.SetReflection( 0.5 );
		b_Material.SetDiffuse( 1.0f );
		b_Material.SetColor( Color( 0.0f,0.0f,0.0f ) );

	}

	int GetType() { return CHECKPLANE; }
	
	Material* GetMaterial() 
	{

		if(true){
			return &w_Material;
		}
		else 
			return &b_Material;
	}
	
	
private:
	vector3 X;
	Material b_Material;
	Material w_Material;
	
};




class Scene
{
public:
	Scene() : m_Primitives( 0 ), m_Primitive( 0 ) {};
	~Scene();
	void InitScene();
	int GetNrPrimitives() { return m_Primitives; }
	Primitive* GetPrimitive( int a_Idx ) { return m_Primitive[a_Idx]; }
private:
	int m_Primitives;
	Primitive** m_Primitive;
};

}; // namespace Raytracer

#endif