/*_____________________________________________________
 |
 |	Intra3D Layer3 :  Scene Graph �ڵ�����ʰȡ
 |                                          
 |  �ļ���MouseSelect.h
 |
 |  ���ܣ������ʰȡ�ɼ��ڵ�(ShapeNode, LightNode)
 |
 |  ���������� ��1999/06/03
 |
 |	Դ������ԣ�������Ӧ�ò���
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


#if !defined (MOUSE_SELECT_H)
#define       MOUSE_SELECT_H

#include "Enum.h"

_declspec(dllexport) void MouseSelectBegin(void);

_declspec(dllexport) BOOL MouseSelectEnd(char *selectedName);

void LoadNameForMouseSelect(char *name);


#endif