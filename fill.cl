kernel void fill(global unsigned int* bitmap, global unsigned int* randomPixels)
{
	int gId = get_global_id(0);
	bitmap[gId] = randomPixels[gId];
}