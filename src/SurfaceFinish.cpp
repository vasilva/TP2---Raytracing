#include "../include/SurfaceFinish.h"

std::ostream &operator<<(std::ostream &out, const SurfaceFinish &sf)
{
	out << "SurfaceFinish Parameters:\n";
	out << "ka = " << sf.getAmbient() << ", kd = " << sf.getDiffuse()
		<< ", ks = " << sf.getSpecular() << ", alpha = " << sf.getAlpha() << "\n";
	out << "kr = " << sf.getReflection() << ", kt = " << sf.getTransmission()
		<< ", ior = " << sf.getIOR();
	return out;
}
