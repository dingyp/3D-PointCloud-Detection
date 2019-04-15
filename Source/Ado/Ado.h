/*########################################################################
	Filename: 	ado.h
	��ado��װ�������桿:
		��Ҫ����������ָ��û�м�ʱ�ͷŵ�bug, ɾ��oleinit()��������Ϊ���ò��ס�
		�����˼�¼����ȡ����.

	��ע�����:
		1. ����������ָ��msado15.dll��ȫ·��,һ����"C:\Program Files\Common
		   Files\System\ADO\"Ŀ¼��.
		2. �������Ҫ��ʼ��COM����,���Ե���CoInitialize(0)����ʼ��; 
		   ��CoUninitialize()�ͷ�;
		3. ��ʹ�ü�¼������ǰ�����ȵ���CAdoConnection::Connect()�������ݿ�,
		   ���Ӻ�ɸ����CAdoRecordSet����ʹ��, �������ر����ݿ�.
		5. �򿪼�¼��ǰ, �ȵ���CAdoRecordSet::SetAdoConnection()�������Ӷ�
		   ��, �������رռ�¼��.
		6. �ڱ�������в����������ı��뾯��:
		   warning: unary minus operator applied to unsigned type, result
		   still unsigned
		   �������˾������,������StdAfx.h�ļ��м�������һ�д����Խ�ֹ��
		   ����: 	#pragma warning(disable:4146) 
  ########################################################################*/
#pragma once
#include <memory>

//C:\Program Files\Common Files\System\ado
#import "msado15.dll" no_namespace rename("EOF","adoEOF") ,rename("BOF","adoBOF")

class CAdoRecordSet;
class CAdoConnection;
class CAdoCommand;
class CAdoParam;

typedef std::tr1::shared_ptr<CAdoRecordSet> CAdoRecSetPtr;
typedef std::tr1::shared_ptr<CAdoConnection> CAdoConnPtr;
typedef std::tr1::shared_ptr<CAdoCommand> CAdoCmdPtr;

/*########################################################################
			------------------------------------------------
						CAdoConnection class
			------------------------------------------------
  ########################################################################*/
class AFX_EXT_CLASS CAdoConnection
{
//����-----------------------------------------------
public:
	CAdoConnection();
    CAdoConnection(const _ConnectionPtr& ptr);
    CAdoConnection& operator = (const _ConnectionPtr& ptr);
	virtual ~CAdoConnection();

    operator _ConnectionPtr () { return m_pConnection; }

//����-----------------------------------------------
public:

    void SetCursorLocateClient(BOOL bUseClient = TRUE);
	void SetConnectTimeOut(long lSeconds);
    /**
     *@����Execute�ڼ�ĳ�ʱ����
     */
    void SetCmdTimeout(long lSeconds);
	BOOL IsOpen();
	BOOL VerifyConnection();
	_ConnectionPtr GetConnection();
	
    CString GetConnectionStr();
//����-----------------------------------------------
public:
	ErrorsPtr GetErrors();
    /**
     * @ ����OLEDB��strConnectΪ�����ַ���
     */
	BOOL Connect(LPCTSTR strConnect, long lOptions = adConnectUnspecified);

    /**
     * @ ����DSN����Ҫ�����û���������
     */
    BOOL Connect(const CString& strDSN, const CString& strUserID,
        const CString& strPW, long lOptions = adConnectUnspecified);
    BOOL Disconnect();

	BOOL Cancel();

    /**
    *@ ʹ�� Connection ����� Execute ��������ִ���κ���ָ�����ӵ� CommandText �����д��͸������Ĳ�ѯ��
    *   ��� CommandText ����ָ�����з��صĲ�ѯ��ִ�в������κν�����洢���µ� Recordset �����С�
    *   �������ǰ��з��صĲ�ѯ�����ṩ�߷��عرյ� Recordset ����
    *   ���ص� Recordset ����ʼ��Ϊֻ��������ǰ���αꡣ����Ҫ���и��๦�ܵ� Recordset ����
    *   Ӧ���ȴ������������������õ� Recordset ����Ȼ��ʹ�� Recordset ����� Open ����ִ�в�ѯ�����������α����͡�
     *@param lpszCmdText �ַ���������Ҫִ�е� SQL ��䡢�������洢���̻��ض��ṩ�ߵ��ı���
     *@param lpRecsAffected ��ѡ�������ͱ������ṩ�����䷵�ز�����Ӱ��ļ�¼��Ŀ��
     *@param lOptions ��ѡ��������ֵ��ָʾ�ṩ��Ӧ��μ��� CommandText ��������Ϊ����ֵ:
        AdCmdText lpszCmdText���ı�����
        AdCmdTable lpszCmdText�Ǳ�����: ADO Ӧ�Զ����� SQL ��ѯ�Ա�� CommandText �����ı��з���������
        AdCmdStoredProc ָʾ�ṩ��Ӧ���洢���̼��� CommandText�� 
        AdCmdUnknown ָʾ CommandText �����е���������δ֪�� 
      *@return ���ִ�гɹ����򷵻�һ���Ϸ���CAdoRecSetPtr����ָ�룻����NULL��
     */
    CAdoRecSetPtr Execute(LPCTSTR lpszCmdText, long* lpRecsAffected = NULL, 
        long lOptions = adCmdText);

    /**
     * @ִ��һ��SQL���
     */
    CAdoRecSetPtr ExecuteSQL(LPCTSTR lpszSQL, long* lpRecsAffected = NULL);
    /**
     * @ ���ر������м�¼
     */
    CAdoRecSetPtr ExecuteQueryTable(LPCTSTR lpszTableName, long* lpRecsAffected = NULL);

    /**
     * @ ��������ʼһ�����񡢻ع������Լ��ύ����
     */
	long BeginTrans();
	BOOL RollbackTrans();
	BOOL CommitTrans();

	_RecordsetPtr OpenSchema(SchemaEnum QueryType);
	CString GetLastError();

    // �����ݿ����ѹ��
	BOOL CompactDB(LPCTSTR strsource, LPCTSTR strdest);

	BOOL ClearTbl(TCHAR* TblName);

protected:
	_ConnectionPtr	m_pConnection;

protected:
    BOOL Release();
};

/*########################################################################
			------------------------------------------------
						CAdoRecordSet class
			------------------------------------------------
  ########################################################################*/

class AFX_EXT_CLASS CAdoRecordSet 
{
//���� ------------------------------------------------
public:
    /**
     *@Ĭ�Ϲ��죺����recordset ���������ӣ���Ҫָ�����Ӷ���󷽿ɴ򿪼�¼��
     */
    CAdoRecordSet();
    /**
     *@����recordset���󣬲��趨��ʼ���Ӷ�����Ҫ��ʾ����Open���򿪼�¼��
     */
    CAdoRecordSet(const _ConnectionPtr& pConnection);

    /**
     *@��һ���Ѿ��򿪵ļ�¼�����죺����ļ�¼�������Ѿ��������ӣ���˲������趨���Ӷ���
     */
    CAdoRecordSet(const _RecordsetPtr& ptr);
    /**
     *@ͬ�ϣ���ֵ��������һ���Ѿ��򿪵ļ�¼�����󸶸���ǰ����
     */
    CAdoRecordSet& operator = (const _RecordsetPtr& ptr);

    virtual ~CAdoRecordSet();

    /**
     *@�򿪼�¼�������򿪵ļ�¼��Ϊ��ʱ��Ҳ�᷵��TRUE
     */
    BOOL Open(LPCTSTR strSQL, long lOption = adCmdText, 
                CursorTypeEnum CursorType = adOpenStatic, 
                LockTypeEnum LockType = adLockOptimistic,
                CursorLocationEnum cursorLocation = adUseClient);

    /**
     *@OpenBySQLText ��select, update�ȳ���sql����򿪼�¼��
     *@OpenByTable ��ĳ��������ȡ�������м�¼
     */
    BOOL OpenBySQLText(LPCTSTR strSQL, CursorTypeEnum CursorType = adOpenStatic, 
        LockTypeEnum LockType = adLockOptimistic,
        CursorLocationEnum cursorLocation = adUseClient);

    BOOL OpenByTable(LPCTSTR strTableName, CursorTypeEnum CursorType = adOpenStatic, 
        LockTypeEnum LockType = adLockOptimistic,
        CursorLocationEnum cursorLocation = adUseClient);

    /**
     *@����ado���Ӷ���
     *@Remarks: �����Ѿ��򿪵�recordset����Connection��������Ϊ ֻ��������
     * �޸ĸ�������Ч
     */
    void SetAdoConnection(const _ConnectionPtr& pConnection);
    void SetAdoConnection(CAdoConnection* pConnection);
    void SetAdoConnection(LPCTSTR lpszConnStr);

    _ConnectionPtr GetActiveConnection();
    CAdoConnPtr GetAdoConnectionPtr();

    CAdoCmdPtr GetAdoCmdPtr();

    /**
     *@�ж��뵱ǰRecordSet��ص����Ӷ����Ƿ��Ѵ�
     */
    BOOL IsConnectionOpen();

    BOOL Close();

    CString GetLastError();
//���� ------------------------------------------------	
public:
	BOOL IsEOF();
	BOOL IsBOF();
	
	long GetState();
	long GetStatus();
	
	long GetRecordCount();
	long GetFieldsCount();

	//CString GetLastError();

	CString GetFieldName(long lIndex);
	DWORD GetFieldType(long lIndex);
	DWORD GetFieldType(LPCTSTR strfield);
	
	long GetFieldAttributes(long lIndex);
	long GetFieldDefineSize(long lIndex);
	long GetFieldActualSize(long lIndex);

	Fields* GetFields();
	FieldPtr GetField(long lIndex);
	const _RecordsetPtr& GetRecordset();
	
    void SetFilter(const CString& strFilter);

    // ֻ��CursorLocationΪadUseClientʱ����֧���������; ������ַ���ȡ����ǰ����
    // �÷���"Field1 ASC, Field2 DESC"
    void SetSort(const CString& strSort);

//��¼�����²��� --------------------------------------
public:
	BOOL AddNew();
	BOOL Update();
	BOOL UpdateBatch();

	BOOL CancelUpdate();
	BOOL CancelBatch(AffectEnum AffectRecords = adAffectAll);
	BOOL Delete(AffectEnum AffectRecords = adAffectCurrent);

//��¼���������� --------------------------------------
public:
	BOOL MoveFirst();
	BOOL MovePrevious();
	BOOL MoveNext();
	BOOL MoveLast();
	BOOL Move(long lRecords, long Start = adBookmarkCurrent);
	
//�������� --------------------------------------------
public:
	BOOL IsOpen();
	BOOL PutCollect(long index, const _variant_t &value);
	BOOL PutCollect(long index, const LPCTSTR &value);
	BOOL PutCollect(long index, const double &value);
	BOOL PutCollect(long index, const float &value);
	BOOL PutCollect(long index, const long &value);
	BOOL PutCollect(long index, const int &value);
	BOOL PutCollect(long index, const short &value);
	BOOL PutCollect(long index, const BYTE &value);

	BOOL PutCollect(LPCTSTR strFieldName, COleDateTime &value);
	BOOL PutCollect(LPCTSTR strFieldName, const _variant_t &value);
	BOOL PutCollect(LPCTSTR strFieldName, const LPCTSTR &value);
	BOOL PutCollect(LPCTSTR strFieldName, const double &value);
	BOOL PutCollect(LPCTSTR strFieldName, const float &value);
	BOOL PutCollect(LPCTSTR strFieldName, const long &value);
	BOOL PutCollect(LPCTSTR strFieldName, const int &value);
	BOOL PutCollect(LPCTSTR strFieldName, const short &value);
	BOOL PutCollect(LPCTSTR strFieldName, const BYTE &value);
	
    BOOL GetCollect(long index,  COleDateTime &value);
    BOOL GetCollect(long index, CString &value);
	BOOL GetCollect(long index, double &value);
	BOOL GetCollect(long index, float &value);
	BOOL GetCollect(long index, long &value);
	BOOL GetCollect(long index, int& value);
	BOOL GetCollect(long index, short &value);
    BOOL GetCollect(long index, BYTE &value);
    BOOL GetCollect(long index,  bool &value);
    BOOL GetCollect(LPCTSTR strFieldName, CString &value);
	BOOL GetCollect(LPCTSTR strFieldName, bool &value);
	BOOL GetCollect(LPCTSTR strFieldName, BYTE &value);

	BOOL GetCollect(LPCTSTR strFieldName, double &value);
	BOOL GetCollect(LPCTSTR strFieldName, float &value);
	BOOL GetCollect(LPCTSTR strFieldName, long &value);
	BOOL GetCollect(LPCTSTR strFieldName, int& value);
	BOOL GetCollect(LPCTSTR strFieldName, short &value);
	BOOL GetCollect(LPCTSTR strFieldName,  COleDateTime &value);
    BOOL GetCollect(LPCTSTR strFieldName, unsigned int& value);

	BOOL Supports(CursorOptionEnum CursorOptions = adAddNew);
	BOOL SetCacheSize(const long& lCacheSize);
	BOOL Cancel();
	BOOL GetValueString(CString &strValue, CString strFieldName);
	BOOL GetValueString(CString &strValue, long lFieldIndex);
	BOOL GetValue(int& val, LPCTSTR fieldName);

	_RecordsetPtr operator =(_RecordsetPtr &pRecordSet);
	CString Format(_variant_t var);

public:
	// ɾ����¼
	BOOL DeleteCurrentRecord(void);
	BOOL DeleteAllRecord(void);	

protected:
    _RecordsetPtr		m_pRecordset;

//��Ա���� --------------------------------------------
protected:
	BOOL Release();
};

/**
 * @Brief: base class for ado command operations. 
 *  Ӧ�ÿ���Ӧʹ�ø����������
 */
class AFX_EXT_CLASS CAdoCommand
{
public:
     /**
     *@ָ��һ�����Ӷ����Command
     */
    void SetAdoConnection(CAdoConnection* pConnnection);
    void SetAdoConnection(const _ConnectionPtr& pConnnection);
    /**
     *@ָ��һ�������ַ�����Command����Command�����ʹ�ø������ַ����Զ���������
     *"[Provider=MSDASQL;] { DSN=name | FileDSN=filename } ; [DATABASE=database;] UID=user; PWD=password"
     */
    void SetAdoConnection(LPCTSTR lpszConnStr);

    CAdoConnPtr GetAdoConnectionPtr();
    _ConnectionPtr GetActiveConnection();

    /**
     *@�ж�Command��������Ӷ���״̬�Ƿ��
     */
    BOOL IsConnectionOpen() /*const*/;
    void SetCmdTimeout(long lSeconds);

    CString GetLastError();

    _ParameterPtr CreateIntParam(int nVal, LPCTSTR lpszParamName = NULL, 
        ParameterDirectionEnum direction = adParamInput);
    _ParameterPtr CreateVarStrParam(const CString& strVal, unsigned int nMaxCharCnt, LPCTSTR lpszParamName = NULL, 
        ParameterDirectionEnum direction= adParamInput);
    _ParameterPtr CreateParam(_variant_t v, DataTypeEnum t, unsigned int nMaxCharCnt, LPCTSTR lpszParamName = NULL, 
        ParameterDirectionEnum direction= adParamInput);
    
    BOOL AppendParam(_ParameterPtr p);

    /**
     *@Create and append parameter
     */
    BOOL CreateAppendParam(_variant_t v, DataTypeEnum t, unsigned int nMaxCharCnt, LPCTSTR lpszParamName = NULL, 
        ParameterDirectionEnum direction= adParamInput);

    BOOL CreateAppendParam(COleDateTime time, LPCTSTR lpszParamName = NULL, 
        ParameterDirectionEnum direction= adParamInput);

    BOOL CreateAppendParam(int n, LPCTSTR lpszParamName = NULL, 
        ParameterDirectionEnum direction= adParamInput);

    _variant_t GetParam(LPCTSTR lpszParamName);
protected:
    _CommandPtr m_pCmd; 

public:
    /**
     *@Ĭ�Ϲ��죬δָ�����Ӷ�����ִ��ǰ����Ҫָ�����Ӷ���
     */
    CAdoCommand();
    /**
     *@�����Ӷ���Ĺ���
     */
    CAdoCommand(CAdoConnection* pConnnection);
    CAdoCommand(const _ConnectionPtr& pConnnection);
};

/**
 *@Brief: CAdoCmdSQLText is designed for executing a sql-command
 */
class AFX_EXT_CLASS CAdoCmdSQLText 
    : public CAdoCommand
{
public:
    CAdoCmdSQLText();
    CAdoCmdSQLText(CAdoConnection* pConnnection);
    CAdoCmdSQLText(const _ConnectionPtr& pConnnection);
    /**
     *@Execute a sql command and returns a record set pointer(smart pointer).
     *@param lpszCmdText sql sentences
     *@return if this call succeeds, a valid record set pointer is returned (but may be a closed set);
     *  if this call failed, a null pointer is returned.
     */
    CAdoRecSetPtr Execute(LPCTSTR lpszCmdText);
    /**
     *@Execute a sql command, and returns true if succeeds, or false.
     *@if succeeded, the @adoRecSetOut will be filled with a valid read-only record set.
     */
    BOOL          Execute(LPCTSTR lpszCmdText, CAdoRecSetPtr& adoRecSetOut);
};

/**
 *@brief: CAdoCmdTable can query a specified table and return all records of that table.
 */
class AFX_EXT_CLASS CAdoCmdTable
    : public CAdoCommand
{
public:
    CAdoCmdTable();
    CAdoCmdTable(CAdoConnection* pConnnection);
    CAdoCmdTable(const _ConnectionPtr& pConnnection);
    /**
     *@same to CAdoCmdSQLText
     */
    CAdoRecSetPtr Execute(LPCTSTR lpszTableName);
    BOOL          Execute(LPCTSTR lpszCmdText, CAdoRecSetPtr& adoRecSetOut);
};

/**
 *@Brief: CAdoCmdStoredProc can execute a stored-procedure.
 * First, call @SetStoredProcName to set name of stored-procedure to execute;
 * Then, call @RefreshParams to clear parameter environment;
 * Then, call @SetParam to set any parameter for the stored-procedure if exist;
 * At last, call @Execute to execute the stored-procedure and get the record set.
 */
class AFX_EXT_CLASS CAdoCmdStoredProc 
    : public CAdoCommand
{
public:
    CAdoCmdStoredProc();
    CAdoCmdStoredProc(CAdoConnection* pConnnection);
    CAdoCmdStoredProc(const _ConnectionPtr& pConnnection);
    /**
     *@����Ҫִ�еĴ洢��������
     */
    void SetStoredProcName(LPCTSTR lpszStoredProcName);

    BOOL RefreshParams();

    /**
     *@���ô洢���̲���
     */
    void SetParam(LPCTSTR lpszParamName, int val);
    void SetParam(LPCTSTR lpszParamName, LPCTSTR val);

    CAdoRecSetPtr Execute();
    BOOL Execute(CAdoRecSetPtr& retRSOut);

    /**
     *@�����ڼ�¼���ĵ��ã������ڻ�ȡ�������������ֵ
     */
    BOOL Execute2();
};

class AFX_EXT_CLASS CAdoParam
{
public:
    CAdoParam(void);
    CAdoParam(_ParameterPtr p);
    ~CAdoParam(void);

    operator _ParameterPtr ()
    {
        return m_paramPtr;
    }

protected:
    _ParameterPtr m_paramPtr;
};
