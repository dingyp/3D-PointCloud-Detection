#pragma	   once
/*
 ���������ļ���ʽ 
 
 �ļ�ͷ{ͷ��С�����ʽ����������}

 ��ͷ1{��ID=1,��Ķ������,������������,�����Ƿ��з�����}
 ������:��������,Ȼ��������������,����з������������Ƿ���������

 ��ͷ2{��ID=2,��Ķ������,������������,�����Ƿ��з�����}
 ������:��������,Ȼ��������������,����з������������Ƿ���������

 ......

*/

//��������ͷ�ļ�
typedef struct _VECTORFILEHEAD{
	int Version;
	int ChunkNum;
	int Resverd1;
	int Resverd2;
	int HeadSize;
}VECTORFILEHEAD;


//��������ļ�
typedef struct _CHUNKHEAD{
	long ChunkID;
	long VectorNum;
	long FaceNum;
	long Normal;
}CHUNKHEAD;

