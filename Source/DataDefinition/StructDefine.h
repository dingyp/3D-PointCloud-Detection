#pragma once
#include <vector>
#include "..\SceneGraph\Vector.h"

struct RadarPoint {
	unsigned int Point_ID;									//���ID��
	unsigned int distance;									//���룬��λ��mm
	double x;												//���x����
	double y;												//���y����
	double z;												//���z����
	double intensity;										//��ķ�����
	unsigned short angleH;									//ˮƽ�Ƕȣ���λ��0.01��
	short angleV;											//��ֱ�Ƕȣ���λ��0.01��
	unsigned char channel;									//ͨ����
	unsigned int timestamp;									//ʱ���
	unsigned char R, G, B;									//��ɫ����

	RadarPoint():R(255),G(0),B(0) {}
};

typedef std::vector<RadarPoint> RadarPointList;				//RadarPoint����

struct Line {												//������ʾ�����ߵĽṹ��
	VECTOR beginpt;											//����ʼ�������
	VECTOR endpt;											//�����˵������
};

typedef enum E_3DMouseMode {								//���������ģʽ��ö������
	MM_ROTAL = 0,											//��ʱ��Ȼctrl�����ˣ�������Ȼ����ת
	MM_MEASURE,												//���
	MM_MEASUREZERO,											//���������ľ���
	MM_3DANGLE												//����3D�ǣ���ʱû���õ�
}E3DMouseMode;

typedef std::vector <Line> LineArray;						//�����߶���
typedef std::vector <VECTOR> PointArray;					//���ĵ�Ķ���

typedef std::vector<VECTOR *> pPointClouds;					//��֡����

enum LINEPLANE_TYPE { NONEVIEW = 0, XOY, XOZ, YOZ };		//�������ö������
enum LINEDRAW_TYPE { GRID = 0, CON_CIRCLE };				//�����������͵�ö�����ͣ�����ͬ��Բ��
enum COLORCODING {
	CODING_ID = 0, CODING_DIS, CODING_X, CODING_Y, CODING_Z, CODING_INTEN, CODING_HANGLE, \
	CODING_VANGLE, CODING_CHAN, CODING_TIME , CODING_MERGE
};				//��ɫ�������Ե�ö������

const char ColorCodingName[10][64] =						//��ɫ�������Ե��ַ�������
{
	"Point ID",												//ID��
	"Distance",												//����
	"X",													//x����
	"Y",													//y����
	"Z",													//z����
	"Intensity",											//������
	"Horizontal Angle",										//ˮƽ�Ƕ�
	"Vertical Angle",										//��ֱ�Ƕ�
	"Channel",												//ͨ����
	"Timestamp",											//ʱ���
};

enum VIEW_DIR { RESET = 0, XP, XS, YP, YS, ZP, ZS };					//��ͼ�����ö������

#define tmp_PI 3.14159265358979
//�Ƕ�ת����
#define Deg2Rad(x) ((x)*tmp_PI/180.0)
//����ת�Ƕ�
#define Rad2Deg(x) ((x)*180.0/tmp_PI)