//*****************************************************************
// �ļ��� :						ModelData.h
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				����ģ�����ݽṹ
// �������� :					
// �޸����� :					
// ���� :						�ƻ�
// �޸��� :						
// ��ϵ��ʽ :					����,�����
//*****************************************************************/
#pragma once
/*****************************************************************
Library Files Included
*****************************************************************/
#include "FilesLoadLib.h"
#include "..\SceneGraph\Vector.h"
#ifdef FILESLOADLIB_EXPORTS
#define CMODELDATA_API __declspec(dllexport)
#else
#define CMODELDATA_API __declspec(dllimport)
#endif

//������ά����VECTOR9ui
struct CMODELDATA_API VECTOR9ui
{
	unsigned int x, y, z; //������index
	unsigned int nx, ny, nz; //������index
	unsigned int uvx, uvy, uvz; //��������index
};

//�����Ͷ�ά����
struct CMODELDATA_API VECTOR2f
{
	float x, y;
};

class CMODELDATA_API CModelData
{
/*****************************************************************
						Routine Definitions
*****************************************************************/
public:
	CModelData(void);
	virtual ~CModelData(void);

	/*��ȡ���ݶ���ָ��*/
	VECTOR * GetPVertex();
	VECTOR * GetPNormal();
	VECTOR9ui * GetPTriangles();
	VECTOR2f * GetPUVCoordinates();

	/*��ȡ������Ԫ����*/
	long GetVertexNum();
	long GetNormalNum();
	long GetTriNum();
	long GetUVNum();
	int  GetMatIndex();

	/*����/�޸Ļ�����Ԫ����*/
	void SetVertexNum(long VerNum);
	void SetNormalNum(long NormalNum);
	void SetTriNum(long TriNum);
	void SetUVNum(long TexNum);
	void SetMatIndex(int MatIndex);

	void SetModelName(char *strName);
	void GetModelName(char *strName);

	/*���������Ԫ����*/
	void CalculateNormal();		//���������ζ�������	

	void CopyModel(CModelData OriModel);

protected:
	void ClearCurModel();
/*****************************************************************
							Variables
*****************************************************************/
protected:
	char m_strName[255];		// ��ǰģ������
	VECTOR * m_pVertex;		//�������
	VECTOR * m_pNormal;		//��������
	VECTOR9ui * m_pTriangles;	//�����ζ���
	VECTOR2f * m_pUVCoordinates; //�����������

	long m_iVertexNum;		//��������
	long m_iNormalNum; //����������
	long m_iTriNum;			//����������
	long m_iUVNum; //����������Ŀ
	int  m_iMatIndex; //���ʱ������

};
