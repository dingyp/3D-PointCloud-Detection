#pragma once
#include "stdafx.h"
#include "BaseStructure.h"//基础的，pcap里面也包含的文件
#define PCAPNG_BLOCK_NUM				150
#define PCAPNG_HEADER_SIGN				0xFEF0010F

//得考虑所有块能够进行强制转换
#pragma pack(1)

typedef struct BlockHead {
	unsigned int BlockType;
	unsigned int BlockLen;
}BlockHead;

/*分节块（强制）剩余部分 0x0a0d0d0a*/
typedef struct SHB {
	unsigned int Order;
	unsigned short Major_Version;
	unsigned short Minor_Version;
	unsigned int Section_length[2];
}SHB;

/*接口描述块（强制）剩余部分 0x00000001*/
typedef struct IDB {
	unsigned short LinkType;
	unsigned short Reserved;
	unsigned int SnapLen;
}IDB;

/*增强报文块剩余部分 0x00000006*/
typedef struct EPB {
	unsigned int Interface_ID;
	unsigned int Timestamp_high;
	unsigned int Timestamp_low;
	unsigned int CapLen;
	unsigned int PacketLen;
}EPB;

/*简单报文块剩余部分 0x00000003*/
typedef struct SPB {
	unsigned int PacketLen;
}SPB;

/*名称解析块剩余部分 0x00000004*/
typedef struct NRB {
	unsigned short Record_Type;
	unsigned short Record_Len;
	//....
};

/*接口统计块剩余部分 0x00000005*/
typedef struct ISB {
	unsigned int Interface_ID;
	unsigned int Timestamp_high;
	unsigned int Timpstamp_low;
}ISB;

/*数据的报头  Header 定义*/
typedef struct data_header {
	unsigned int sign;								//标识符，固定为PCAPNG_HEADER_SIGN(0xFEF0010F)
	unsigned short protocol;						//协议版本，当前
	unsigned char ratio_length;						//距离比例，实际距离值=读出数据×此数据 (mm)
	unsigned char brand[3];							//品牌商代码，大写字母及数字表示，长度不足末尾补”\0”OLE：欧镭； HDL：华达科捷； GST：巨星； GZR：国自机器人
	unsigned char type[12];							//销售型号字符串，字符串以”\0”结尾
	unsigned short intra_type;						//内部型号代码
	unsigned short version_hard;					//硬件版本
	unsigned short version_soft;					//软件版本
	unsigned int timestamp;							//时间戳（待定），单位为 ms。表示 24 小时内的时、分、秒、毫秒
	unsigned char system_status[8];					//系统状态码（详细含义待定）
}data_header;

/*数据块定义*/
typedef struct Block {
	unsigned short angle;							//角度，无符号整数。有效范围：0~35999
													//表示单位 0.01° / LSB，表示范围 0°~359.99°
													//注：此项值如果大于 0xFF00，则表示本数据块无效，必须忽略

	unsigned short close_range;						//近距离，无符号整数。
													//表示距离由包头的距离比例决定，即“读取的数值×包头的距
													//离比例”得到实际距离（单位：mm）。
													//0xFF00~0xFFFF 保留，表示特殊意义。
													//0xFFFF：代表距离无效。
													//0xFFFE：距离超过量程。
													//0xFFFD：距离过近，属于盲区。

	unsigned char close_intensity;					//近距离反射率，表示范围 0~100、128 及以上的值表示高反射率目标
	unsigned short far_range;						//远距离，意义同近距离反射率。对于单反射目标，此项填 0
	unsigned char far_intensity;					//远距离反射率，意义同近距离反射率。对于单反射目标，此项填 0
}Block;

#pragma pack()