/*_____________________________________________________
 |
 |	Intra3D Layer1 :  Algebra �������֮һ
 |                                          
 |  �ļ���Rotation.h
 |
 |  ���ܣ���Ԫ����ת����
 |
 |  ���������� ��1999/01/10
 |
 |	Դ������ԣ������˵������ٲ���
 |_____________________________________________________*/

/*_____________________________________________________
 |  
 |	���� OpenGL �Ľ���ʽ��άͼ�������������
 |
 |	**   Intra3D 2.0 for Windows 9X/NT  **
 |
 |	          ����Ȩ�ˣ����� 
 |	
 |	�㽭��ѧ CAD&CG �����ص�ʵ���� (310027)
 |_____________________________________________________*/


#if !defined(ROTATION_H)
#define		 ROTATION_H

#include "Vector.h"

/*______________________ ROTATION ͨ�� QUATERNION ʵ�� _____________________*/


// ������ת ROTATION �任, ROTATION ��һ����ָ axis ��һ��
// �� ROTATION �ṹ��ʾ��תʱ��(axis, angle) �� -(axis, angle) �ȼ�

class AFX_EXT_CLASS ROTATION
{
public:
	VECTOR axis ;	// Unit Vector  
	float  angle;	// 0 - 360 degree

	// �����һ���� ROTATION 
	ROTATION(float fX=0.0f, float fY=0.0f, float fZ=1.0f, float fAngle=0.0f)
	{ axis.x=fX; axis.y=fY; axis.z=fZ; angle=fAngle; VectorNormalize(&axis); }

	// �����һ���� ROTATION 
	ROTATION(VECTOR Axis, float fAngle=0.0f)
	{ axis=Axis; angle=fAngle; VectorNormalize(&axis); }
};

// ROTATION ��ˣ�����ֵ = R1 * R2  (��ִ�� R1 ��ת����ִ�� R2 ��ת)
// Ҫ�� R1 R2 �ȹ�һ��

ROTATION _declspec(dllexport) RotationMultiply( ROTATION R1,  ROTATION R2);


ROTATION _declspec(dllexport) operator *  ( ROTATION R1,  ROTATION R2);

// V ���� R �任��Ϊ V', �������� V'
// Ҫ�� R �ȹ�һ��
VECTOR _declspec(dllexport) VectorTransform(const VECTOR V, const ROTATION R);

// �� ROTATION �ṹ��ʾ�� Matrix
// Ҫ�� R �ȹ�һ��
void _declspec(dllexport) RotationToMatrix(float M[16], const ROTATION R);


/*_______________ �����˽���Ԫ��ϸ�ڵĳ���Ա�������¿� _____________________*/



// ������Ԫ�� QUATERNION
// �� QUATERNION �ṹ��ʾ��תʱ��(x,y,z,w) �� -(x,y,z,w) �ȼ�

class QUATERNION
{
public:
	float x, y, z, w;
	QUATERNION(float fX=0.0f, float fY=0.0f, float fZ=1.0f, float fW=0.0f)
	{ x=fX; y=fY; z=fZ; w=fW; }
};


// ��Ԫ����ģ������ֵ = |A|
float QuaternionMagnitude(const QUATERNION A);

// ��Ԫ���һ��
// ��� |A|=0�����ֵ = A�����ֵ = A/(|A|)
void  QuaternionNormalize(QUATERNION *A);

// ��Ԫ������
// ��� |A|=0�����ֵ = A���������ֵ = A ����
void  QuaternionInverse(QUATERNION *A);

// ��Ԫ�鹲�����ֵ = A �Ĺ���
void  QuaternionConjugate(QUATERNION *A);

// ��Ԫ����ӣ�����ֵ = A + B
QUATERNION QuaternionAdd(const QUATERNION A, const QUATERNION B);

QUATERNION operator +   (const QUATERNION A, const QUATERNION B);

// ��Ԫ�����������ֵ = A - B
QUATERNION QuaternionSub(const QUATERNION A, const QUATERNION B);

QUATERNION operator -   (const QUATERNION A, const QUATERNION B);

// ��Ԫ�����ţ�����ֵ = s * A 
QUATERNION QuaternionScale(const QUATERNION A, const float s);

QUATERNION operator *     (const QUATERNION A, const float s);

QUATERNION QuaternionScale(const float s, const QUATERNION A);

QUATERNION operator *     (const float s, const QUATERNION A);

// ��Ԫ����ˣ�����ֵ = A * B
QUATERNION QuaternionMultiply(const QUATERNION A, const QUATERNION B);

QUATERNION operator *     (const QUATERNION A, const QUATERNION B);  

// Spherical Linear Interpolation Between two Quaternions, 0 < t < 1
QUATERNION QuaternionSlerp(const QUATERNION from, const QUATERNION to, float t);

/*_____________________________________________________
 |                                                     
 |   Ϊ��߼������ܣ����±任�������ٶ�
 |
 |   ������� R Q V �Ѿ������� Normalize ����    
 |_____________________________________________________*/


/*------------------------------------------------------------------*/
/*----------------------  QUATERNION �� ROTATION  ------------------*/

// �� ROTATION �ṹ��ʾ�� QUATERNION
QUATERNION RotationToQuaternion(const ROTATION R);

// �� QUATERNION �ṹ��ʾ�� ROTATION
ROTATION  QuaternionToRotation(const QUATERNION Q);

/*------------------------------------------------------------------*/
/*----------------------  QUATERNION �� Matrix  --------------------*/

// �� QUATERNION �ṹ��ʾ�� Matrix
void QuaternionToMatrix(float M[16], const QUATERNION Q);

// �� Matrix �ṹ��ʾ�� QUATERNION
// Ҫ�� Matrix ��һ����ת����,�����ܵõ���ȷ���
void MatrixToQuaternion(QUATERNION *Q, const float M[16]);

/*------------------------------------------------------------------*/
/*-----------------------  QUATERNION �� VECTOR  -------------------*/


// ��ʸ��������ά�ռ��һ�㣩��ʾ����Ԫ��
QUATERNION VectorToQuaternion(const VECTOR V);

// ����Ԫ����鲿��ʸ����ʾ
VECTOR  QuaternionToVector(const QUATERNION Q);

// V ���� Q �任��Ϊ V', �������� V'
VECTOR	VectorTransform(const VECTOR V, const QUATERNION Q);

#endif