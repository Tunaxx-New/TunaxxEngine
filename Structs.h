const int VPOS_COUNT   = 3;										//vector position count
const int MATRIX_COUNT = (VPOS_COUNT + 1) * (VPOS_COUNT + 1);	//Matrix size
const int SCALE_POSES  = (VPOS_COUNT + 1);						//Positions (y)
const int FVEC_COUNT   = 4;										//Face points MAX count
union vecn
{
	double pos[VPOS_COUNT];
};
union face
{
	cl_int count = 0;
	vecn v [FVEC_COUNT];
	vecn vt[FVEC_COUNT];
	vecn vn[FVEC_COUNT];
};