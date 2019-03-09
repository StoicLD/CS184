#include "transforms.h"

#include "CGL/matrix3x3.h"
#include "CGL/vector2D.h"
#include "CGL/vector3D.h"
#include <cmath>
namespace CGL {

Vector2D operator*(const Matrix3x3 &m, const Vector2D &v) {
	//为了让3X3的矩阵和2维向量操作，临时创建了一个3D向量
	Vector3D mv = m * Vector3D(v.x, v.y, 1);
	return Vector2D(mv.x / mv.z, mv.y / mv.z);
}

Matrix3x3 translate(float dx, float dy) {
	// Part 3: Fill this in.
	return Matrix3x3(1, 0, dx,
	                 0, 1, dy,
	                 0, 0, 1);
}

Matrix3x3 scale(float sx, float sy) {
	// Part 3: Fill this in.
	return Matrix3x3(sx, 0, 0,
	                 0, sy, 0,
	                 0, 0, 1);
}

// The input argument is in degrees counterclockwise
Matrix3x3 rotate(float deg) {
	// Part 3: Fill this in.
	//注意deg是度数，但是C++的cmath库中接受的是弧度
	float pi = 3.141592;
    float rad = (deg / 180) * pi;
	return Matrix3x3(cos(rad), -sin(rad), 0,
	                 sin(rad), cos(rad), 0,
	                 0 ,0, 1);
}

}
