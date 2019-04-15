#pragma once

/**************************************************���������ʾ����*******************************************************/
enum 
{
	WatchDogCount = 99999
};

enum ErrorCodeType{
	RaView_NO_ERROR				 = 0x00000000,	//��������

	//�����Ƕ�ȡ������
	ALREADY_Sen_Con				 = 0x00000001,	//�Ѿ��д���������
	ER_CREATESENCON				 = 0x00000002,	//��������������
	ER_BuildBeforeInit			 = 0x00000003,	//��������֮ǰ��û�а󶨴�����
	ALReadyDisFileConnection	 = 0x00000004,	//��Ҫ�ر��ļ�����ʱ�Ѿ�Ϊ��
	ALReadyFileConnection		 = 0x00000005,	//�����ļ���ȡ����ʱ���Ѿ�����
	ER_CreateFileConnection		 = 0x00000006,	//�����ļ�����ʧ��
	ALReadyDisSenConnection		 = 0x00000007,	//��Ҫ�رմ�������ʱ�Ѿ�Ϊ��
	ER_NotRawProcess			 = 0x00000008,	//��������ʱ��û�д�����
	ER_FileReaderIsNULL			 = 0x00000009,	//��ʹ���ļ���ȡ�������ǿ�
	ER_SensorReaderIsNULL		 = 0x0000000A,	//��ʹ�ô��ж�ȡ�������ǿ�
	ER_NOTConnectToSensor		 = 0x0000000B,	//û���ܹ����ϴ�����
	ER_GetFileNameNotInPcap		 = 0x0000000C,	//�������ļ���ȡģʽ����õ��ļ�·��

	//����Ϊ�ļ���ȡ����
	ER_OPENSTREAMNOTSTOP		 = 0x00100000,	//���ٶ�û��ͣ�µ�ʱ��׼�����ļ�
	ER_OPENSTREAMNOTNULL		 = 0x00100001,	//���ļ�ָ���Ѿ��򿪵��������׼�����ļ�
	ER_CREATEPCAPREADER			 = 0x00100002,	//������ȡ�ļ�����
	ER_OpenPcapFile				 = 0x00100003,	//�򿪶�ȡ�ļ�ʧ��
	ER_ALREADYOFFSETTABLE		 = 0x00100004,	//�Ѿ��������ļ�����
	ER_OFFSETTABLENOTSTOP		 = 0x00100005,	//�����������ʱ��û��ֹͣ
	ER_CREATEOFFSETTABLE		 = 0x00100006,	//����������ʧ��
	ER_CREATEOFFSETTABLEA		 = 0x00100007,	//����������ʧ��+1
	ER_IDX_TABLEEMPTY			 = 0x00100008,	//�ļ�����vector��С==0
	ER_DESTORYFILEREADERSTART	 = 0x00100009,	//�˳�whileѭ��
	ER_OpenReaderISOpen			 = 0x0010000A,	//�����Ѿ��򿪵Ķ�ȡ��
	ER_NOTBUNDRETURNFUC			 = 0x0010000B,	//���ļ�û�а󶨻ص�����
	INVAILDSPEED				 = 0x0010000C,	//�����Ƿ��ٶ�
	ER_FILESTREAMNULL			 = 0x0010000D,	//��ȡ�ļ�ʱ�ļ�ָ��Ϊ��
	ER_READFILENOOFFSET			 = 0x0010000E,	//��ȡ�ļ�ʱû��������
	ER_INDEXOUTRANGE			 = 0x0010000F,	//��ȡ������������Χ
	ER_CURINDEXOUTRANGE			 = 0x00100010,	//��ǰcurindex�±곬����Χ
	ER_NODESTORYFILREADER		 = 0x00100011,	//û���˳�whileѭ��
	ER_InitPointRawProcess		 = 0x00100012,	//��ʼ����ʱ������ָ��Ϊ��
	ER_PcapReaderIsNULL			 = 0x00100013,	//ѭ���ڲ����ֶ�ȡ��Ϊ�յĴ���
	ER_pBuffNotCreated			 = 0x00100014,	//����pBuff��ʱ����ִ���
	ER_FileReaderPointIsNULL	 = 0x00100015,	//�ļ��Ķ���������ʱΪ��
	ER_SensorReaderPointIsNULL	 = 0x00100016,	//�����Ķ���������ʱΪ��
	ER_NotCreateTimer			 = 0x00100017,	//���붨ʱ������
	ER_CannotCreateOffset		 = 0x00100018,	//���ܹ�����������

	//����Ϊ��������ȡ����
	ER_noDestorySensorReader	 = 0x00200000,	//û���˳�whileѭ��
	ALReadySaveReader			 = 0x00200001,	//�뽨�������ļ�����ʱ�����䲻Ϊ��
	ER_SaveReaderIsNULL			 = 0x00200002,	//�����ļ��߳��Ѿ��ǿ�
	ER_PCAP_FindAllDevs			 = 0x00200003,	//��ȡ�豸�б��ʱ����ִ���
	ER_NotFindSpecDev			 = 0x00200004,	//Ѱ���ض��豸ʧ��
	ER_OnlyWorkInEthernet		 = 0x00200005,	//����ֻ����̫���Ϲ���
	ER_CannotCompileInPcap		 = 0x00200006,	//�����޷�����
	ER_CannotSetFilterInPcap	 = 0x00200007,	//���ù���������
	ER_ReadingInPackets			 = 0x00200008,	//��ȡ������
	ER_InvaildSensorMode		 = 0x00200009,	//��Ч�Ĵ���ģʽ
	ER_OutTimeOfConnect			 = 0x0020000A,	//������ʱ
	ER_UnSureOfProtocol			 = 0x0020000B,	//��Ч��Э��
	ER_UnSetTheProtocol			 = 0x0020000C,	//û������Э��

	//��������ȡ��д����
	ER_BuildSaverNULL			 = 0x00210000,	//����SaveReaderʱ�����ǿ�
	ER_SaveReaderNotNULL		 = 0x00210001,	//��Init��ʱ�������Ѿ���ʼʹ��
	ER_SaveStreamNotNULL		 = 0x00210002,	//�ڴ��������ļ�
	ER_CreatePcapSaver			 = 0x00210003,	//����д��ʱ���ɹ�
	ER_OpenSaveFile				 = 0x00210005,	//�򿪱���������
	BeginSaveIDIsNotNULL		 = 0x00210006,	//д�ļ��߳��Ѿ����ˣ���Ϊ��
	ER_SaveFullCallBackIsNULL	 = 0x00210007,	//�����ļ�̫���Ļص�����ָ���ǿ�

	//����Ϊԭʼ���ݴ�����
	ER_SensorFrameAleadyClear	 = 0x00300000,	//����֡��ʱ��֡�Ѿ�����
	RE_NotBindCallBackFunc		 = 0x00300001,	//start�߳̿�ʼʱ����û�а󶨻ص�����
	ER_InfoSizeIsNotRight		 = 0x00300002,	//�����ݵĴ�С�����ϣ��˳�
	ER_BlockHeadIsNotRight		 = 0x00300003,	//blockͷ�������϶���

};