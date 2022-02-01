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

kernel void matrix(global union _face* f, int sizev, int sizep, global double* matrix, int sizem) 
{
	size_t gId = get_global_id(0);
	for (int i = 0; i < sizev; ++i) 
	{
		for (int j = 0; j < sizep; ++j) 
		{
			double coord = 0;
			for (int k = 0; k < sizep; ++k) {
				coord += matrix[j * sizem + k] * f[gId].v[i].pos[k];
			}
			coord += f[gId].v[i].pos[sizep];
			f[gId].v[i].pos[j] = coord;
		}
	}
}