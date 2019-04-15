/*_____________________________________________________
 |
 |	Intra3D Layer1 :  Container ����
 |                                          
 |  �ļ���Container.h
 |
 |  ���ܣ������������á�ɾ��
 |
 |  ���������� ��1999/05/17
 |
 |	Դ������ԣ�������Ӧ�ò���
 |_____________________________________________________*/

/*_____________________________________________________
 |
 |  2003��10��30 �ƻ� �޸�
 |_____________________________________________________*/



#if !defined (CONTAINER_H)
#define       CONTAINER_H

/*________________________________________________________________

					�������ü������ڴ������

	��ȷ��������������ﳤ���ĵ�һ������ο� Microsoft COM ����
  ________________________________________________________________*/

class AFX_EXT_CLASS Container
{
public:
	// �ڴ�������ʱ��������Ϊ 0
	Container(void){ m_cRefrenceCount=0; }
	virtual ~Container(){}; //������������Ϊ�麯������delete this ��ʱ��Ż���ü̳���������������ƻ�

	// ������ʱ�����Ӽ�����
    void AddRef(void){ m_cRefrenceCount++; }

	// ���������Ϊ0����ɾ������
    void Release(void)
	{ 
		m_cRefrenceCount--; 
		if(m_cRefrenceCount <= 0) 
		{
			FinalRelease();	// �����ͷ������ݵĶ���
			delete this;
		}
	}

protected:

	// �����ͷ������ݵĶ���
	virtual void FinalRelease(void);

protected:
	int	m_cRefrenceCount;	// ������
};


#endif

/*___________________________________________________________________________

					Why - Container - Class

The Container class is the base class for all objects in a scene; 
where an object is an entity that you can place in the scene graph. 
A Container provides reference counting for all its children.

Many kinds of data objects in Intra3D can be placed in a hierarchical 
scene graph. Using instancing, an object can be referenced multiple times. 
Scene graphs can become quite complex, which can cause problems if you're 
not careful. Deleting objects can be a particularly dangerous operation, 
for example, if you delete an object that another object still references.

Within each Container is a counter that keeps track of the number of objects 
referencing a particular instance. Reference counting provides a bookkeeping 
mechanism that makes object deletion safe: an object should never be deleted 
if its reference count is greater than zero. In general, you should only 
unreference an object in case it is referenced by another object.

It is just as important, however, not to unreference an object that has not 
been referenced. 

Each Container is created with a reference count of 1. 

When object A is attached to object B, the reference count of A is incremented. 
Additionally, if A replaces a previously referenced object C, the reference 
count of C is decremented. 

See Also SGI Cosmo3D. Microsoft COM
							Understand ?
_____________________________________________________________________________*/

