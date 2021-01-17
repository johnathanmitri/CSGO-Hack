// pch.cpp: source file corresponding to the pre-compiled header

#include "pch.h"

float toRad(float dec)
{
	return (float)(dec * (M_PI / 180));
}

// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.
