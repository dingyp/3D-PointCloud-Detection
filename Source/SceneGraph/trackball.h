/*_____________________________________________________
 |
 |	Intra3D Layer1 :  Algebra �������֮һ
 |                                          
 |  �ļ���Trackball.h
 |
 |  ���ܣ����-��������ת
 |
 |  ���������� ��1999/02/01
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

//���������㷨
//���ã��������ת��ʸ������ת�Ƕ�
#if !defined(TRACKBALL_H)
#define		 TRACKBALL_H

#include "Vector.h"

class Trackball
{
public:
	Trackball(void);
   ~Trackball(void);

public:
	// �����������Ϊ���ڵ�����
	void SetTrackWindow(int iWidth, int iHeight);

	// �����������Ϊ��������(centerX, centerY) 
	void SetTrackWindow(int iWidth, int iHeight, int iCenterX, int iCenterY);

	// ����հ������ʱ������ (mouseX, mouseY)
	void Start(int iMouseX, int iMouseY);

	// ����ƶ� (dx,dy), ������ת�� axis ��Ƕ� angle 
	void Tracking(int iDx, int iDy, VECTOR *axis, float *fAngle);

protected:

	// �����(mouseX, mouseY)ͶӰ������һ�� P
	// ����ֵ��Ϊ���ĵ�P��ʸ��
	VECTOR MouseToVector(int iMouseX, int iMouseY);

	int    m_iWidth,   m_iHeight;	// ���ڵĿ��
	int    m_iCenterX, m_iCenterY;	// ����������
	int    m_iRadius;				// ������뾶
	int    m_iMouseX,  m_iMouseY;	// ���λ��

	VECTOR m_start;	// �հ���ʱ�����ͶӰʸ��
	VECTOR m_stop;	// �ƶ�(dx,dy)ʱ�����ͶӰʸ��

};

#endif // TRACKBALL_H
