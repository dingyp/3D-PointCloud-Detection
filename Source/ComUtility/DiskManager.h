#pragma once

namespace fsutility
{
class AFX_EXT_CLASS CDiskManager
{
public:
	CDiskManager(void);
	~CDiskManager(void);

	//��ȡ���̵Ŀ��д�С����λ�� M
	static float GetDiscSpaceFree(CString path);
};

}
