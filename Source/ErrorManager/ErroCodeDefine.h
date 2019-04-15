#pragma once

/**************************************************定义错误提示代码*******************************************************/
enum 
{
	WatchDogCount = 99999
};

enum ErrorCodeType{
	RaView_NO_ERROR				 = 0x00000000,	//正常返回

	//以下是读取管理部分
	ALREADY_Sen_Con				 = 0x00000001,	//已经有传感器连接
	ER_CREATESENCON				 = 0x00000002,	//创建传感器出错
	ER_BuildBeforeInit			 = 0x00000003,	//建立连接之前还没有绑定处理类
	ALReadyDisFileConnection	 = 0x00000004,	//想要关闭文件连接时已经为空
	ALReadyFileConnection		 = 0x00000005,	//创建文件读取器的时候已经有了
	ER_CreateFileConnection		 = 0x00000006,	//创建文件连接失败
	ALReadyDisSenConnection		 = 0x00000007,	//想要关闭传感连接时已经为空
	ER_NotRawProcess			 = 0x00000008,	//建立连接时还没有处理类
	ER_FileReaderIsNULL			 = 0x00000009,	//想使用文件读取器发现是空
	ER_SensorReaderIsNULL		 = 0x0000000A,	//想使用传感读取器发现是空
	ER_NOTConnectToSensor		 = 0x0000000B,	//没有能够连上传感器
	ER_GetFileNameNotInPcap		 = 0x0000000C,	//不是在文件读取模式下想得到文件路径

	//以下为文件读取部分
	ER_OPENSTREAMNOTSTOP		 = 0x00100000,	//在速度没有停下的时候准备打开文件
	ER_OPENSTREAMNOTNULL		 = 0x00100001,	//在文件指针已经打开的情况下又准备打开文件
	ER_CREATEPCAPREADER			 = 0x00100002,	//创建读取文件出错
	ER_OpenPcapFile				 = 0x00100003,	//打开读取文件失败
	ER_ALREADYOFFSETTABLE		 = 0x00100004,	//已经建立了文件索引
	ER_OFFSETTABLENOTSTOP		 = 0x00100005,	//建立索引表的时候没有停止
	ER_CREATEOFFSETTABLE		 = 0x00100006,	//创建索引表失败
	ER_CREATEOFFSETTABLEA		 = 0x00100007,	//创建索引表失败+1
	ER_IDX_TABLEEMPTY			 = 0x00100008,	//文件索引vector大小==0
	ER_DESTORYFILEREADERSTART	 = 0x00100009,	//退出while循环
	ER_OpenReaderISOpen			 = 0x0010000A,	//打开了已经打开的读取器
	ER_NOTBUNDRETURNFUC			 = 0x0010000B,	//打开文件没有绑定回调函数
	INVAILDSPEED				 = 0x0010000C,	//遇到非法速度
	ER_FILESTREAMNULL			 = 0x0010000D,	//读取文件时文件指针为空
	ER_READFILENOOFFSET			 = 0x0010000E,	//读取文件时没有索引表
	ER_INDEXOUTRANGE			 = 0x0010000F,	//读取的索引表超出范围
	ER_CURINDEXOUTRANGE			 = 0x00100010,	//当前curindex下标超出范围
	ER_NODESTORYFILREADER		 = 0x00100011,	//没有退出while循环
	ER_InitPointRawProcess		 = 0x00100012,	//初始化的时候处理类指针为空
	ER_PcapReaderIsNULL			 = 0x00100013,	//循环内部出现读取器为空的错误
	ER_pBuffNotCreated			 = 0x00100014,	//创建pBuff的时候出现错误
	ER_FileReaderPointIsNULL	 = 0x00100015,	//文件阅读类在启动时为空
	ER_SensorReaderPointIsNULL	 = 0x00100016,	//传感阅读类在启动时为空
	ER_NotCreateTimer			 = 0x00100017,	//申请定时器出错
	ER_CannotCreateOffset		 = 0x00100018,	//不能够创建索引表

	//以下为传感器读取部分
	ER_noDestorySensorReader	 = 0x00200000,	//没有退出while循环
	ALReadySaveReader			 = 0x00200001,	//想建立保存文件连接时发现其不为空
	ER_SaveReaderIsNULL			 = 0x00200002,	//保存文件线程已经是空
	ER_PCAP_FindAllDevs			 = 0x00200003,	//获取设备列表的时候出现错误
	ER_NotFindSpecDev			 = 0x00200004,	//寻找特定设备失败
	ER_OnlyWorkInEthernet		 = 0x00200005,	//程序只在以太网上工作
	ER_CannotCompileInPcap		 = 0x00200006,	//数据无法编译
	ER_CannotSetFilterInPcap	 = 0x00200007,	//设置过滤器出错
	ER_ReadingInPackets			 = 0x00200008,	//读取包出错
	ER_InvaildSensorMode		 = 0x00200009,	//无效的传感模式
	ER_OutTimeOfConnect			 = 0x0020000A,	//窃听超时
	ER_UnSureOfProtocol			 = 0x0020000B,	//无效的协议
	ER_UnSetTheProtocol			 = 0x0020000C,	//没有设置协议

	//传感器读取中写部分
	ER_BuildSaverNULL			 = 0x00210000,	//建立SaveReader时返回是空
	ER_SaveReaderNotNULL		 = 0x00210001,	//在Init的时候发现类已经开始使用
	ER_SaveStreamNotNULL		 = 0x00210002,	//在创建保存文件
	ER_CreatePcapSaver			 = 0x00210003,	//创建写流时不成功
	ER_OpenSaveFile				 = 0x00210005,	//打开保存器出错
	BeginSaveIDIsNotNULL		 = 0x00210006,	//写文件线程已经有了，不为空
	ER_SaveFullCallBackIsNULL	 = 0x00210007,	//保存文件太满的回调函数指针是空

	//以下为原始数据处理部分
	ER_SensorFrameAleadyClear	 = 0x00300000,	//发送帧的时候帧已经满了
	RE_NotBindCallBackFunc		 = 0x00300001,	//start线程开始时发现没有绑定回调函数
	ER_InfoSizeIsNotRight		 = 0x00300002,	//该数据的大小不符合，退出
	ER_BlockHeadIsNotRight		 = 0x00300003,	//block头部不符合定义

};