#include "Matrix4.h"

Matrix4::Matrix4() {
	//zeros
	for (int i = 0; i < MATRIX4_NUM_ELEM; i++) {
		m[i] = 0.0f;
	}
}

Matrix4 Matrix4::Identity() {
	Matrix4 result;
	result.m[0] = 1.0f;
	result.m[5] = 1.0f;
	result.m[10] = 1.0f;
	result.m[15] = 1.0f;
	return result;
}

Matrix4 Matrix4::Translate(float x, float y, float z) {
	//start with identity
	Matrix4 result = Matrix4::Identity();
	result.m[12] = x;
	result.m[13] = y;
	result.m[14] = z;
	return result;
}

Matrix4 Matrix4::Scale(float x, float y, float z) {
	Matrix4 result;
	result.m[0] = x;
	result.m[5] = y;
	result.m[10] = z;
	result.m[15] = 1.0f;
	return result;
}

Matrix4 Matrix4::RotateX(float angle) {
	Matrix4 result;
	result.m[0] = 1.0f;
	result.m[5] = cos(angle);
	result.m[9] = -sin(angle);
	result.m[6] = sin(angle);
	result.m[10] = cos(angle);
	result.m[15] = 1.0f;
	return result;
}

Matrix4 Matrix4::RotateY(float angle) {
	Matrix4 result;
	result.m[0] = cos(angle);
	result.m[8] = sin(angle);
	result.m[5] = 1.0f;
	result.m[2] = -sin(angle);
	result.m[10] = cos(angle);
	result.m[15] = 1.0f;
	return result;
}

Matrix4 Matrix4::RotateZ(float angle) {
	Matrix4 result;
	result.m[0] = cos(angle);
	result.m[4] = -sin(angle);
	result.m[1] = sin(angle);
	result.m[5] = cos(angle);
	result.m[10] = 1.0f;
	result.m[15] = 1.0f;
	return result;
}

Matrix4 Matrix4::Orthographic(float left, float right, float top, float bottom, float near, float far) {
	Matrix4 result;
	result.m[0] = 2.0f / (right - left);
	result.m[5] = 2.0f / (top - bottom);
	result.m[10] = -2.0f / (far - near);
	result.m[15] = 1.0f;
	result.m[12] = -(right + left) / (right - left);
	result.m[13] = -(top + bottom) / (top - bottom);
	result.m[14] = -(far + near) / (far - near);
	return result;
}

Matrix4 Matrix4::Perspective(float left, float right, float top, float bottom, float near, float far) {
	Matrix4 result;
	result.m[0] = 2.0f * near / (right - left);
	result.m[8] = (right + left) / (right - left);
	result.m[5] = 2.0f * near / (top - bottom);
	result.m[9] = (top + bottom) / (top - bottom);
	result.m[10] = -(far + near) / (far - near);
	result.m[14] = -2.0f * far * near / (far - near);
	result.m[11] = -1.0f;
	//result.m[15] = 1.0f;
	return result;
}

Matrix4 Matrix4::Perspective(float fov, float aspect, float near, float far) {
	float f = 1.0f / tan(fov / 2.0f);
	Matrix4 result;
	result.m[0] = f / aspect;
	result.m[5] = f;
	result.m[10] = (far + near) / (near - far);
	result.m[14] = 2 * far*near / (near - far);
	result.m[11] = -1.0f;
	return result;
}

Matrix4 Matrix4::operator*(const Matrix4& other) {
	Matrix4 result;
	//result.m[0] = m[0] * other.m[0] + m[4] * other.m[1] + m[8] * other.m[2] + m[12] * other.m[3];
	for (int c = 0; c < 4; c++) {
		for (int r = 0; r < 4; r++) {
			result.m[r + c * 4] = 0.0f;
			for (int i = 0; i < 4; i++) {
				result.m[r + c * 4] += m[r + i * 4] * other.m[i + c * 4];
			}
		}
	}
	return result;
}

Vertex Matrix4::operator*(const Vertex& other) {
	Vertex result;
	result.x = other.x * m[0] + other.y * m[4] + other.z * m[8] + other.w * m[12];
	result.y = other.x * m[1] + other.y * m[5] + other.z * m[9] + other.w * m[13];
	result.z = other.x * m[2] + other.y * m[6] + other.z * m[10] + other.w * m[14];
	result.w = other.x * m[3] + other.y * m[7] + other.z * m[11] + other.w * m[15];
	/*for (int i = 0; i < MATRIX_SIZE; i++) {
		result.v[i] = 0.0f;
		for (int j = 0; j < VECTOR4_SIZE; j++) {
			result.v[i] += other.v[j] * m[i + j * 4];
		}
	}*/
	return result;
}

Matrix4 Matrix4::operator*(float scalar) {
	Matrix4 result;
	for (int i = 0; i < MATRIX4_NUM_ELEM; i++) {
		result.m[i] = m[i] * scalar;
	}
	return result;
}

Matrix4 Matrix4::transpose() {
	Matrix4 result;
	result.m[0] = m[0];
	result.m[1] = m[4];
	result.m[2] = m[8];
	result.m[3] = m[12];
	result.m[4] = m[1];
	result.m[5] = m[5];
	result.m[6] = m[9];
	result.m[7] = m[13];
	result.m[8] = m[2];
	result.m[9] = m[6];
	result.m[10] = m[10];
	result.m[11] = m[14];
	result.m[12] = m[3];
	result.m[13] = m[7];
	result.m[14] = m[11];
	result.m[15] = m[15];
	return result;
}