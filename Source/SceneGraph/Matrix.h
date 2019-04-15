/*_____________________________________________________
 |  �ļ���Matrix.h
 |
 |  ���ܣ���������
 |
 |	Դ������ԣ������˺�ϻ����
 |_____________________________________________________*/


#if !defined(MATRIX_H)
#define		 MATRIX_H

#include "Vector.h"
#include "Rotation.h"

class _declspec(dllexport) Matrix
{
public:
	Matrix();
	float m[16];
	float *getMatrix(){return (float *)m;}
};

// ������ 
// ���������A(i,j)
// ���������M=A
void _declspec(dllexport) MatrixCopy(float *M, 
			const float *A, const int i=4, const int j=4); 

// ������� 
// ���������A(i,j)  B(i,j)
// ���������M=A+B
void _declspec(dllexport) MatrixAdd(float *M, 
		   const float *A, const float *B,
		   const int  i=4, const int  j=4); 

// ������� 
// ���������A(i,j)  B(i,j)
// ���������M=A-B
void _declspec(dllexport) MatrixSub(float *M, 
		   const float *A, const float *B,
		   const int  i=4, const int  j=4); 

// ������� 
// ���������M1(i1,j1)   M2(i2,j2)
// ���������M (i1,j2) = M1*M2
// Լ��������i2=j1�����򲻼��� M 
void _declspec(dllexport) MatrixMultiply(float *M, 
			    const float *M1, const float *M2, 
			    const int  i1, const int  j1, 
			    const int  i2, const int  j2);

// ����ת�� 
// ���������A(i,j)
// ���������M(j,i) = A ��ת��
void _declspec(dllexport) MatrixTranspose(float *M, 
				 const float *A, const int i=4, const int j=4); 

// ����ת�� 
// ���������A(i,j)
// ���������A(j,i) = A ��ת��
void _declspec(dllexport) MatrixTranspose(float *A, const int i=4, const int j=4); 

// �������� 
// ���������A(n, n)
// ���������M = A ����
// ����ֵ  ��true ��ʾ�ɹ���false ��ʾ������󲻴��ڣ�M ���ֲ��䣩
bool _declspec(dllexport) MatrixInverse(float *M, 
			   const float *A, const int n=4);

// �������� 
// ���������A(n, n)
// ���������A = A ����
// ����ֵ  ��true ��ʾ�ɹ���false ��ʾ������󲻴��ڣ�A ���ֲ��䣩
bool _declspec(dllexport) MatrixInverse(float *A, const int n=4);

// �����һ�� 
// ���������A(n, n)
// ���������A = A �Ĺ�һ������
void _declspec(dllexport) MatrixIdentity(float *A, const int n=4);


// V ���� M �任��Ϊ V', �������� V'
VECTOR _declspec(dllexport) VectorTransform(const VECTOR V, const float  M[16]);

QUATERNION _declspec(dllexport) QuaternionTransform(const QUATERNION V,const float M[16]);

#endif