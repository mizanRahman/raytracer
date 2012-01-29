
#ifndef I_SURFACE_H
#define I_SURFACE_H

#include "CVector.h"

namespace Raytracer {

class Surface
{

public:
	// constructor / destructors
	Surface( int a_Width, int a_Height );
	~Surface();

	// member data access
    Pixel* GetBuffer() { return m_Buffer; }
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }

	void Clear( Pixel a_Color );

private:
	// Attributes
	Pixel* m_Buffer;	
	int m_Width, m_Height;	
	
};

}; // namespace Raytracer

#endif