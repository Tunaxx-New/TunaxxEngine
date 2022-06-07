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
// This is function bitmapping the game of life
//
kernel void conwayGoL(global unsigned int* bitmap, short width, short height, global unsigned int* newBitmap)
{
	//Coordinates, index
	int gId = get_global_id(0);
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