#if!defined(ENUM_H)
#define ENUM_H
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>

//�����б���Ŀ
#define FONTLIST    256

//��������
enum GLFontType
{
	GL_FONT_SOLID=0,
	GL_FONT_LINE
}; 

enum RotateMode
{
	SG_NORMALMODE=0,
	SG_EASYMODE
};

enum InteractionMode
{
	IM_ROTATE_NORMAL = 0,
	IM_ROTATE_BOXONLY,
	IM_SELECT,
	IM_TRANSLATE,
};

//////////////////////////////
/*PI����*/
const double PI=3.1415926; 
const float DELTA_ROT=0.00001f;//��ת��С������2004��2��6�޸�

/* �ڵ����ͳ���*/ 
//Scene Graph �ڵ����
const int SCENE_NODE_BASE  =  0;

//���ʽڵ�
const int MATERIAL_NODE    =  1;

/********��Դ�ڵ㼰��������****************/
const int LIGHT_NODE_BEGIN = 10; 
const int POINT_LIGHT_NODE = 12; //���Դ�ڵ�
const int DIR_LIGHT_NODE   = 13; //ƽ�й�Դ�ڵ�
const int SPOT_LIGHT_NODE  = 14; //�۹�Դ�ڵ�
//������...
const int LIGHT_NODE_END   = 19;
/*************************************/


/********����ڵ㼰��������*************/
const int CAMERA_NODE_BEGIN= 20;
const int CAMERA_NODE      = 21;//����ڵ�
//������...
const int CAMERA_NODE_END  = 29;
/*************************************/

/***********�任�ڵ���*************/
const int TRANSFORM_NODE   = 30;
/*************************************/

/*************��ڵ���*************/
const int GROUP_NODE       =40;
/*************************************/

/************����ڵ㼰������********/
const int SCENEOBJECT_NODE = 50;
const int GEOMETRY_NODE    =51;
const int TEX_GEOMETRY_NODE=52;
const int MARK_NODE = 53;
const int PATH_NODE = 54;
const int CUBOID_NODE = 55;
const int SPHERE_NODE = 56;
const int KNIFE_NODE = 57;
const int RADARPOINTS_NODE = 58;
const int RADARAXIS_NODE = 59;
const int STL_GEOMETRY_NODE = 65;
const int OBJ_GEOMETRY_NODE = 65;
/**************************************/

//��꽻����ʽ����
const int NO_MOUSE_INTERACTION = 60;
const int MOUSE_SELECT = 61;
const int MOUSE_ROTATE = 62;
const int MOUSE_FEEDBACK = 63;
const int MOUSE_TRANSLATE = 64;

//���ֽڵ�
const int TEXT_NODE = 70;

/*__________________________________________________

        ����ΪProjection Mode����
  __________________________________________________*/

typedef enum E_PROJ_MODE{ PERSPECTIVE = 0,
                        ORTHO
    }PROJ_MODE;

/////////////////////////////
typedef enum E_MOUSE_MODE{MM_MOUSE_HAND=0 , 
						  MM_MOUSE_MOVE,
					      MM_MYDEFAULT
	}MOUSE_MODE;

/*_____________________________________________________
  
	����Ϊ���ȵĳ���
  _____________________________________________________*/

const int NODE_NAME_LENGTH = 32;	// �ڵ����ֵ��ַ�������
// ����������
static const int TREE_DEPTH=50;

typedef struct STRUCT_BOUND_BOX{
	float CenterX;
	float CenterY;
	float CenterZ;
	float XRange;
	float YRange;
	float ZRange;
}BOUND_BOX;

#endif