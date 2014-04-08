#ifndef ISENTEK_TYPEDEFS_H
#define ISENTEK_TYPEDEFS_H

#include <stddef.h>
#include <stdint.h>

#ifndef EXTERN_C_BEGIN
#  ifdef __cplusplus
#    define EXTERN_C_BEGIN	extern "C" {
#  else
#    define EXTERN_C_BEGIN
#  endif
#endif

#ifndef EXTERN_C_END
#  ifdef __cplusplus
#    define EXTERN_C_END	}
#  else
#    define EXTERN_C_END
#  endif
#endif

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef TRUE
#  define TRUE	1
#endif

#ifndef FALSE
#  define FALSE	0
#endif

typedef enum _SensorType {
	SensorType_Accelerometer = 0x000000001,
	SensorType_MagneticField = 0x00000002,
	SensorType_Gyroscope = 0x00000004,
	SensorType_Gravity = 0x00000009,
	SensorType_LinearAcceleration = 0x0000000a,
	SensorType_RotationVector = 0x0000000b,
	SensorType_All = 0xffffffff,
} SensorType;

#define Vi(vc,i)	(vc).v[(i)]
#define Vx(vc)		(vc).v[0]
#define Vy(vc)		(vc).v[1]
#define Vz(vc)		(vc).v[2]

typedef struct _Vector {
	float v[3];
} Vector;

#define Ai(an,i)	(an).v[(i)]
#define Ax(an)		(an).v[0]
#define Ay(an)		(an).v[1]
#define Az(an)		(an).v[2]
#define Aroll		Ax
#define Apitch		Ay
#define Ayaw		Az

typedef struct _Angular {
	float v[3];
} Angular;

#define Mij(mt,i,j)	(mt).v[(i)][(j)]
#define M11(mt)		(mt).v[0][0]
#define M12(mt)		(mt).v[0][1]
#define M13(mt)		(mt).v[0][2]
#define M21(mt)		(mt).v[1][0]
#define M22(mt)		(mt).v[1][1]
#define M23(mt)		(mt).v[1][2]
#define M31(mt)		(mt).v[2][0]
#define M32(mt)		(mt).v[2][1]
#define M33(mt)		(mt).v[2][2]

typedef struct _Matrix {
	float v[3][3];
} Matrix;

#define Qi(qt,i)	(qt).v[(i)]
#define Qx(qt)		(qt).v[0]
#define Qy(qt)		(qt).v[1]
#define Qz(qt)		(qt).v[2]
#define Qw(qt)		(qt).v[3]

typedef struct _Quaternion {
	float v[4];
} Quaternion;

#endif // ISENTEK_TYPEDEFS_H