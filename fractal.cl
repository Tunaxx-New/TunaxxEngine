//--Complex number struct--------------------------------------------------
// This class have math functions and two fields
struct Complex
{
	float real;
	float imag;
};

//
// initializeComplex initializing complex number
// @param1 number where it changes
// @param2 real part
// @param3 imaginary part
void initializeComplex(struct Complex* c, float r, float i)
{
	c->real = r;
	c->imag = i;
}



//
// sumComplex summarazing two complex numbers
// @param1 number to add (changes)
// @param2 number source add from
void sumComplex(struct Complex* c, struct Complex* cAdd)
{
	c->real += cAdd->real;
	c->imag += cAdd->imag;
}



//
// lengthComplex len
// @param1 number
//
float lengthComplex(struct Complex* c)
{
	return sqrt(c->real * c->real + c->imag * c->imag);
}



//
// absComplex len
// @param1 number
//
float absComplex(struct Complex* c)
{
	return c->real * c->real + c->imag * c->imag;
}



//
// sqrComplex pows complex number in square
// @param1 number to square (changes)
//
void sqrComplex(struct Complex* c)
{
	float temp = c->real;
	c->real = c->real * c->real - c->imag * c->imag;
	c->imag = 2 * temp * c->imag;
}
//-------------------------------------------------------------------------



//--Function-with-color----------------------------------------------------

//
// Funciton converts rgb to hex
//
unsigned int rgbToHex(short r, short g, short b)
{
	if (r > 255)
		r = 255;
	if (g > 255)
		g = 255;
	if (b > 255)
		b = 255;

	if (r < 0)
		r = 0;
	if (g < 0)
		g = 0;
	if (b < 0)
		b = 0;

	return r * 0x010000 + g * 0x000100 + b;
}

//-------------------------------------------------------------------------



//
// This is function bitmapping the fractal
//
kernel void mandelbrotSet(global unsigned int* bitmap, short width, short height, float paramReal, float paramImag)
{
	//Coordinates, index, complex numbers
	int gId = get_global_id(0);
	float x = (float)(gId % width - width / 2) / 500.0;
	float y = (float)(gId / width - height / 2) / 500.0;
	struct Complex c;
	struct Complex z;

	//Initializing
	initializeComplex(&c, x, y);
	initializeComplex(&z, paramImag, 0);
	bitmap[gId] = 0;

	//Cycle with alpha paint in complex function increase length
	for (float j = 0; j < 2; j += 0.1f) {
		int i;
		for (i = 0; i < 80; i++)
		{
			sqrComplex(&z);
			sumComplex(&z, &c);
			if (absComplex(&z) <= j)
				break;
		}
		short hue = 255 - 255 * i / 80;
		hue /= 20;
		bitmap[gId] += rgbToHex(hue, hue, hue);
	}
}
kernel void hello()
{
	printf("Hello, world!");
}