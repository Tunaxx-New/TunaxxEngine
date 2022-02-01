#if defined(cl_khr_fp64)
#  pragma OPENCL EXTENSION cl_khr_fp64: enable
#elif defined(cl_amd_fp64)
#  pragma OPENCL EXTENSION cl_amd_fp64: enable
#else
#  error double precision is not supported
#endif

union _vecn
{
    double pos[3];
};
union _face
{
    int count;
	union _vecn v [4];
	union _vecn vt[4];
	union _vecn vn[4];
};
//global union _face* f, int sizev, int sizep
kernel void pp(global union _face* f, int sizev, int sizep) 
{
	size_t gId = get_global_id(0);
	for (int i = 0; i < sizev; i++) 
	{
		for (int j = 0; j < sizep; j++) 
		{
			f[gId].v[i].pos[j]++;
		}
	}
}