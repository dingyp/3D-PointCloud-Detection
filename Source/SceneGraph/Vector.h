/*_____________________________________________________
 |                                          
 |  �ļ���Vector.h
 |
 |  ���ܣ���άʸ������
 |
 |	Դ������ԣ������˵������ٲ���
 |_____________________________________________________*/


#if !defined(VECTOR_H)
#define		 VECTOR_H

// ������άʸ�� VECTOR
struct VECTOR
{
	float x,y,z;
};

struct VECTOR3ui
{
	unsigned int x,y,z;
};

// ʸ����ģ������ֵ = |A|
 float _declspec(dllexport) VectorMagnitude(const VECTOR A);

// ʸ����һ��, ��� |A|=0�����ֵ = A���������ֵ = A/(|A|)
 void  _declspec(dllexport) VectorNormalize(VECTOR *A);

// ʸ����ӣ�����ֵ = A + B
 VECTOR _declspec(dllexport) VectorAdd(const VECTOR A, const VECTOR B);
 VECTOR _declspec(dllexport) operator+(const VECTOR A, const VECTOR B);

// ʸ�����������ֵ = A - B
 VECTOR _declspec(dllexport) VectorSub(const VECTOR A, const VECTOR B);
 VECTOR _declspec(dllexport) operator-(const VECTOR A, const VECTOR B);

// ʸ�����ţ�����ֵ = s * A
 VECTOR _declspec(dllexport) VectorScale(const VECTOR A, const float  s);
 VECTOR _declspec(dllexport) operator * (const VECTOR A, const float  s);
 VECTOR _declspec(dllexport) VectorScale(const float  s, const VECTOR A);
 VECTOR _declspec(dllexport) operator * (const float  s, const VECTOR A);

// ʸ�����������ֵ = A x B
 VECTOR _declspec(dllexport) VectorCross(const VECTOR A, const VECTOR B);
 VECTOR _declspec(dllexport) operator * (const VECTOR A, const VECTOR B);

// ʸ�����������ֵ = A . B
 float  _declspec(dllexport) VectorDot(const VECTOR A, const VECTOR B);
 float  _declspec(dllexport) operator^(const VECTOR A, const VECTOR B);


#endif