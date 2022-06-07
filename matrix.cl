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



#define MATRIX_SIZE 4
//
// This function for blitting union object with params
//
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