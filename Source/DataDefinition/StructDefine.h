#pragma once
#include <vector>
#include "..\SceneGraph\Vector.h"

struct RadarPoint {
	unsigned int Point_ID;									//点的ID号
	unsigned int distance;									//距离，单位：mm
	double x;												//点的x坐标
	double y;												//点的y坐标
	double z;												//点的z坐标
	double intensity;										//点的反射率
	unsigned short angleH;									//水平角度，单位：0.01°
	short angleV;											//垂直角度，单位：0.01°
	unsigned char channel;									//通道号
	unsigned int timestamp;									//时间戳
	unsigned char R, G, B;									//颜色定义

	RadarPoint():R(255),G(0),B(0) {}
};

typedef std::vector<RadarPoint> RadarPointList;				//RadarPoint队列

struct Line {												//保存显示画的线的结构体
	VECTOR beginpt;											//线起始点的坐标
	VECTOR endpt;											//线终了点的坐标
};

typedef enum E_3DMouseMode {								//鼠标所处的模式的枚举类型
	MM_ROTAL = 0,											//此时虽然ctrl按下了，但是仍然是旋转
	MM_MEASURE,												//测距
	MM_MEASUREZERO,											//测量到零点的距离
	MM_3DANGLE												//测量3D角，暂时没有用到
}E3DMouseMode;

typedef std::vector <Line> LineArray;						//画的线队列
typedef std::vector <VECTOR> PointArray;					//画的点的队列

typedef std::vector<VECTOR *> pPointClouds;					//多帧点云

enum LINEPLANE_TYPE { NONEVIEW = 0, XOY, XOZ, YOZ };		//网格方向的枚举类型
enum LINEDRAW_TYPE { GRID = 0, CON_CIRCLE };				//画的网格类型的枚举类型（网格、同心圆）
enum COLORCODING {
	CODING_ID = 0, CODING_DIS, CODING_X, CODING_Y, CODING_Z, CODING_INTEN, CODING_HANGLE, \
	CODING_VANGLE, CODING_CHAN, CODING_TIME , CODING_MERGE
};				//颜色编码属性的枚举类型

const char ColorCodingName[10][64] =						//颜色编码属性的字符串数组
{
	"Point ID",												//ID号
	"Distance",												//距离
	"X",													//x坐标
	"Y",													//y坐标
	"Z",													//z坐标
	"Intensity",											//反射率
	"Horizontal Angle",										//水平角度
	"Vertical Angle",										//垂直角度
	"Channel",												//通道号
	"Timestamp",											//时间戳
};

enum VIEW_DIR { RESET = 0, XP, XS, YP, YS, ZP, ZS };					//视图方向的枚举类型

#define tmp_PI 3.14159265358979
//角度转弧度
#define Deg2Rad(x) ((x)*tmp_PI/180.0)
//弧度转角度
#define Rad2Deg(x) ((x)*180.0/tmp_PI)