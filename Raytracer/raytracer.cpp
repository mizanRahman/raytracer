
#include "raytracer.h"
#include "scene.h"
#include "CVector.h"
#include "windows.h"
#include "winbase.h"
#include "stdio.h"
namespace Raytracer {

Ray::Ray( vector3& a_Origin, vector3& a_Dir ) : 
	m_Origin( a_Origin ), 
	m_Direction( a_Dir )
{
}

Engine::Engine():tr_count(0)
{
	m_Scene = new Scene();
}

Engine::~Engine()
{
	delete m_Scene;
}

void Engine::SetTarget( Pixel* a_Dest, int a_Width, int a_Height )
{
	// set pixel buffer address & size
	m_Dest = a_Dest;
	m_Width = a_Width;
	m_Height = a_Height;
}


Primitive* Engine::Raytrace( Ray& a_Ray, Color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist )
{
	tr_count++;
	
	if (a_Depth > 10/*TRACEDEPTH*/) return 0;
	// trace primary ray
	a_Dist = 1000000.0f;
	vector3 pi;
	Primitive* prim = 0;
	int result;//intersect or not
	// find the nearest intersection
	for ( int s = 0; s < m_Scene->GetNrPrimitives(); s++ )
	{
		Primitive* pr = m_Scene->GetPrimitive( s );
		int res;
		if (res = pr->Intersect( a_Ray, a_Dist )) 
		{
			prim = pr;
			result = res; // 0 = miss, 1 = hit, -1 = hit from inside primitive
		}
	}
	// no hit, terminate ray
	if (!prim) return 0;
	// handle intersection
	if (prim->IsLight())
	{
		// we hit a light, stop tracing
		a_Acc = Color( 1, 1, 1 );//light color should be white
	}
	else
	{
		// determine color at point of intersection
		pi = a_Ray.GetOrigin() + a_Ray.GetDirection() * a_Dist;
		// trace lights
		for ( int l = 0; l < m_Scene->GetNrPrimitives(); l++ )
		{
		   
			Primitive* p = m_Scene->GetPrimitive( l );
			if (p->IsLight()) 
			{
				Primitive* light = p;
				float shade = 1.0f;

				//calculate shading

				vector3 L = ((Sphere*)light)->GetCentre() - pi;
				float tdist = LENGTH( L );
				L.Normalize();
				Ray r = Ray( pi + L * EPSILON, L );
				for ( int s = 0; s < m_Scene->GetNrPrimitives(); s++ )
				{
					Primitive* pr = m_Scene->GetPrimitive( s );
					if ((pr != light) && (pr->Intersect( r, tdist )))
					{
						shade = 0;
						break;
					}
				}

				// calculate diffuse shading
				vector3 N = prim->GetNormal( pi );
				if (prim->GetMaterial()->GetDiffuse() > 0)
				{
					float dot = DOT( N, L );
					if (dot > 0) //<90
					{
						float diff = dot * prim->GetMaterial()->GetDiffuse()*shade;
						// add diffuse component to ray color
						a_Acc += diff * prim->GetMaterial()->GetColor() * light->GetMaterial()->GetColor();
					}
				}

				// determine specular component
				if (prim->GetMaterial()->GetSpecular() > 0)
				{
					// point light source: sample once for specular highlight
					vector3 V = a_Ray.GetDirection();
					vector3 R = L - 2.0f * DOT( L, N ) * N;
					float dot = DOT( V, R );
					if (dot > 0)
					{
						float spec = powf( dot, 20 ) * prim->GetMaterial()->GetSpecular() * shade;
						// add specular component to ray color
						a_Acc += spec * light->GetMaterial()->GetColor();
					}
				}



			}
		}

		// calculate reflection
		float refl = prim->GetMaterial()->GetReflection();
		if (refl > 0.0f)
		{
			vector3 N = prim->GetNormal( pi );
			vector3 R = a_Ray.GetDirection() - 2.0f * DOT( a_Ray.GetDirection(), N ) * N;
			if (a_Depth < TRACEDEPTH) 
			{
				Color rcol( 0, 0, 0 );
				float dist;
				Raytrace( Ray( pi + R * EPSILON, R ), rcol, a_Depth + 1, a_RIndex, dist );
				a_Acc += refl * rcol * prim->GetMaterial()->GetColor();
			}
		}

		// calculate refraction
		/*float refr = prim->GetMaterial()->GetRefraction();
		if ((refr > 0) && (a_Depth < TRACEDEPTH))
		{
			float rindex = prim->GetMaterial()->GetRefrIndex();
			float n = a_RIndex / rindex;//ua/uc
			vector3 N = prim->GetNormal( pi ) * (float)result;
			float cosI = -DOT( N, a_Ray.GetDirection() );
			float sinI = 1.0f - cosI * cosI;
			float cosR = 1.0f - n*n*(1-sinI * sinI);
			if (cosR > 0.0f)
			{
				
				Color rcol( 0, 0, 0 );
				float dist;
				Raytrace( Ray( pi + T * EPSILON, T ), rcol, a_Depth + 1, rindex, dist );
				a_Acc += rcol;
			}
		}*/
		 
	}

	//tracer->getTraceCount();
	// return pointer to primitive hit by primary ray
	return prim;
}

void Engine::InitRender()
{
	// set firts line to draw to
	m_CurrLine = 20;
	// set pixel buffer address of first pixel
	m_PPos = 20 * m_Width;
	// screen plane in world space coordinates
	m_WX1 = -4, m_WX2 = 4, m_WY1 = m_SY = 3, m_WY2 = -3;
	// calculate deltas for interpolation
	m_DX = (m_WX2 - m_WX1) / m_Width;
	m_DY = (m_WY2 - m_WY1) / m_Height;
	m_SY += 20 * m_DY;
	// allocate space to store pointers to primitives for previous line
	m_LastRow = new Primitive*[m_Width];
	memset( m_LastRow, 0, m_Width * 4 );
}

bool Engine::Render()
{
	vector3 o( 0, 0, -5 );
	int msecs = GetTickCount();
	Primitive* lastprim = 0;
	for ( int y = m_CurrLine; y < (m_Height); y++ )
	{
		m_SX = m_WX1;
		// render pixels for current line
		for ( int x = 0; x < m_Width; x++ )
		{
			Color acc( 0, 0, 0 );
			vector3 dir = vector3( m_SX, m_SY, 0 ) - o;
			NORMALIZE( dir );
			Ray r( o, dir );
			float dist;
			Primitive* prim = Raytrace( r, acc, 1, 1.0f, dist );
			int red = (int)(acc.r * 256);
			int green = (int)(acc.g * 256);
			int blue = (int)(acc.b * 256);
			if (red > 255) red = 255;
			if (green > 255) green = 255;
			if (blue > 255) blue = 255;
			m_Dest[m_PPos++] = (red << 16) + (green << 8) + blue;
			m_SX += m_DX;
		}
		m_SY += m_DY;
		if ((GetTickCount() - msecs) > 100) 
		{
			m_CurrLine = y + 1;
			return false;
		}
	}
	return true;
}

}; 