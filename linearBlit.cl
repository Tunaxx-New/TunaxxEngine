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

//Engine--------------------------------------------------------------------------------------------------------------
//
// Structs in cl and cpp
//
#define OBJ_COORD_COUNT      3
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
//--------------------------------------------------------------------------------------------------------------------