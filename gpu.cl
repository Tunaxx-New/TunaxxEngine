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
//Fractal-Field---------------------------------------------------------------------------------

//
// This is function bitmapping the game of life
//
kernel void conwayGoL(global unsigned int* bitmap, short width, short height, global unsigned int* newBitmap, int offset)
{
	//Coordinates, index
	//int gId = get_global_id(0);
	for (int gId = get_global_id(0); gId < height * width; gId += offset)
	{

	int x = gId % width;
	int y = gId / width;
	if (bitmap[gId] == rgbToHex(64, 64, 64))
		bitmap[gId] = 0;

	//Cycling 9 cells and counting life
	int life = 0;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			int x0 = x + j;
			int y0 = y + i;
			//If index out of border
			if (x0 < 0) {
				x0 = width - 1;
			}
			else if (x0 >= width) {
				x0 = 0;
			}
			if (y0 < 0) {
				y0 = height - 1;
			}
			else if (y0 >= height) {
				y0 = 0;
			}
			if (bitmap[y0 * width + x0] == rgbToHex(255, 255, 255))
				life++;
		}
	}

	//Rules of cell
	short hue = 0;
	if (bitmap[gId] != 0)
		hue = 255;
	newBitmap[gId] = bitmap[gId];
	if (bitmap[gId] == rgbToHex(255, 255, 255)) {
		if (life > 4 || life < 3)
			hue = 64;
	}
	else {
		if (life == 3)
			hue = 255;
	}

	newBitmap[gId] = rgbToHex(hue, hue, hue);

	}
}
//GOL-Field-----------------------------------------------------------------------

kernel void fill(global unsigned int* bitmap, global unsigned int* randomPixels)
{
	int gId = get_global_id(0);
	bitmap[gId] = randomPixels[gId];
}

//GPU-ENGINE-RASTER---------------------------------------------------------------
//
// Structs in cl and cpp
//
#define OBJ_COORD_COUNT 3
union vertex
{
    float coordinates[OBJ_COORD_COUNT];
};
//
// Brezenham algorithm for line
//
void line(global unsigned int* bitmap, int x1, int y1, int x2, int y2, short width, int size) {
    const int deltaX = abs(x2 - x1);
    const int deltaY = abs(y2 - y1);
    const int signX = x1 < x2 ? 1 : -1;
    const int signY = y1 < y2 ? 1 : -1;
    int error = deltaX - deltaY;
    int index = y2 * width + x2;
		if (0 <= index && index < size)
			bitmap[index] = rgbToHex(255, 255, 255);
    while(x1 != x2 || y1 != y2) 
   {
        int index = y1 * width + x1;
			if (0 <= index && index < size)
				bitmap[index] = rgbToHex(255, 255, 255);

        int error2 = error + error;
        if(error2 > -deltaY) 
        {
            error -= deltaY;
            x1 += signX;
        }
        if(error2 < deltaX) 
        {
            error += deltaX;
            y1 += signY;
        }
    }

}


//
// This function for blitting union object with params
//
kernel void blitLine(global unsigned int* bitmap, short width, short height, int size, global union vertex* vertex, global int* faces)
{
	size_t gId = get_global_id(0);
	if (gId > 0) {
		float x  = vertex[faces[gId]].coordinates[0];
		float y  = vertex[faces[gId]].coordinates[1];
		float x0 = vertex[faces[gId - 1]].coordinates[0];
		float y0 = vertex[faces[gId - 1]].coordinates[1];

		x += width / 2;
		y += height / 2;
		x0 += width / 2;
		y0 += height / 2;
		
		x = (int)x;
		y = (int)y;
		x0 = (int)x0;
		y0 = (int)y0;

		line(bitmap, x0, y0, x, y, width, size);
	}
}



//
// This function for blitting union object with params
//
#define MATRIX_SIZE 4
kernel void matrix(global union vertex* vertex, global float* matrix, global union vertex* result)
{
	size_t gId = get_global_id(0);
	for (int j = 0; j < MATRIX_SIZE - 1; j++)
	{
		float summarazing = 0;
		for (int k = 0; k < MATRIX_SIZE - 1; k++)
		{
			summarazing += vertex[gId].coordinates[k] * matrix[j * MATRIX_SIZE + k];
		}
		summarazing += matrix[j * MATRIX_SIZE + MATRIX_SIZE - 1];

		result[gId].coordinates[j] = summarazing;
	}

	float summarazing = 0;
	for (int k = 0; k < MATRIX_SIZE - 1; k++)
	{
		summarazing += vertex[gId].coordinates[k] * matrix[3 * MATRIX_SIZE + k];
	}
	summarazing += matrix[3 * MATRIX_SIZE + MATRIX_SIZE - 1];
	for (int k = 0; k < MATRIX_SIZE - 1; k++)
		if (summarazing != 0) {
			result[gId].coordinates[k] /= summarazing;
		}
}
//--------------------------------------------------------------------------------------------------------------------