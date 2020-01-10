import numpy as np
from vectorfields import CustomUV2D

ufunc = lambda x,y: np.ones(x.shape)
vfunc = lambda x,y:np.sin(x)

vf = CustomUV2D(ufunc, vfunc, size=[8,8], resolution=[16,16])
vf.save_fga("test.fga")
#vfd3D = vf.to_3d()
#vfd3D.save_vf("test.vf")
#vfd3D.save_fga("test.fga")