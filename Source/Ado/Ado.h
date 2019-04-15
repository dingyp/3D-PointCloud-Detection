/*########################################################################
	Filename: 	ado.h
	【ado封装类修正版】:
		主要修正了智能指针没有及时释放的bug, 删除oleinit()函数，因为觉得不妥。
		增加了记录集存取函数.

	【注意事项】:
		1. 必须在下面指明msado15.dll的全路径,一般在"C:\Program Files\Common
		   Files\System\ADO\"目录下.
		2. 程序必须要初始化COM环境,可以调用CoInitialize(0)来初始化; 
		   用CoUninitialize()释放;
		3. 在使用记录集对象前必须先调用CAdoConnection::Connect()连接数据库,
		   连接后可给多个CAdoRecordSet对象使用, 用完后请关闭数据库.
		5. 打开记录集前, 先调用CAdoRecordSet::SetAdoConnection()设置连接对
		   象, 用完后请关闭记录集.
		6. 在编译过程中不用理会下面的编译警告:
		   warning: unary minus operator applied to unsigned type, result
		   still unsigned
		   如果不想此警告出现,可以在StdAfx.h文件中加入这样一行代码以禁止此
		   警告: 	#pragma warning(disable:4146) 
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
//构建-----------------------------------------------
public:
	CAdoConnection();
    CAdoConnection(const _ConnectionPtr& ptr);
    CAdoConnection& operator = (const _ConnectionPtr& ptr);
	virtual ~CAdoConnection();

    operator _ConnectionPtr () { return m_pConnection; }

//属性-----------------------------------------------
public:

    void SetCursorLocateClient(BOOL bUseClient = TRUE);
	void SetConnectTimeOut(long lSeconds);
    /**
     *@调用Execute期间的超时设置
     */
    void SetCmdTimeout(long lSeconds);
	BOOL IsOpen();
	BOOL VerifyConnection();
	_ConnectionPtr GetConnection();
	
    CString GetConnectionStr();
//操作-----------------------------------------------
public:
	ErrorsPtr GetErrors();
    /**
     * @ 连接OLEDB，strConnect为连接字符串
     */
	BOOL Connect(LPCTSTR strConnect, long lOptions = adConnectUnspecified);

    /**
     * @ 连接DSN，需要输入用户名和密码
     */
    BOOL Connect(const CString& strDSN, const CString& strUserID,
        const CString& strPW, long lOptions = adConnectUnspecified);
    BOOL Disconnect();

	BOOL Cancel();

    /**
    *@ 使用 Connection 对象的 Execute 方法，可执行任何在指定连接的 CommandText 参数中传送给方法的查询。
    *   如果 CommandText 参数指定按行返回的查询，执行产生的任何结果将存储在新的 Recordset 对象中。
    *   如果命令不是按行返回的查询，则提供者返回关闭的 Recordset 对象。
    *   返回的 Recordset 对象始终为只读、仅向前的游标。如需要具有更多功能的 Recordset 对象，
    *   应首先创建具有所需属性设置的 Recordset 对象，然后使用 Recordset 对象的 Open 方法执行查询并返回所需游标类型。
     *@param lpszCmdText 字符串，包含要执行的 SQL 语句、表名、存储过程或特定提供者的文本。
     *@param lpRecsAffected 可选，长整型变量，提供者向其返回操作所影响的记录数目。
     *@param lOptions 可选，长整型值，指示提供者应如何计算 CommandText 参数，可为下列值:
        AdCmdText lpszCmdText是文本命令
        AdCmdTable lpszCmdText是表名称: ADO 应自动生成 SQL 查询以便从 CommandText 命名的表中返回所有行
        AdCmdStoredProc 指示提供者应按存储过程计算 CommandText。 
        AdCmdUnknown 指示 CommandText 参数中的命令类型未知。 
      *@return 如果执行成功，则返回一个合法的CAdoRecSetPtr智能指针；否则NULL。
     */
    CAdoRecSetPtr Execute(LPCTSTR lpszCmdText, long* lpRecsAffected = NULL, 
        long lOptions = adCmdText);

    /**
     * @执行一条SQL语句
     */
    CAdoRecSetPtr ExecuteSQL(LPCTSTR lpszSQL, long* lpRecsAffected = NULL);
    /**
     * @ 返回表中所有记录
     */
    CAdoRecSetPtr ExecuteQueryTable(LPCTSTR lpszTableName, long* lpRecsAffected = NULL);

    /**
     * @ 事务处理：开始一个事务、回滚事务以及提交事务
     */
	long BeginTrans();
	BOOL RollbackTrans();
	BOOL CommitTrans();

	_RecordsetPtr OpenSchema(SchemaEnum QueryType);
	CString GetLastError();

    // 对数据库进行压缩
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
//构建 ------------------------------------------------
public:
    /**
     *@默认构造：创建recordset 对象，无连接，需要指定连接对象后方可打开记录集
     */
    CAdoRecordSet();
    /**
     *@创建recordset对象，并设定初始连接对象，需要显示调用Open来打开记录集
     */
    CAdoRecordSet(const _ConnectionPtr& pConnection);

    /**
     *@从一个已经打开的记录集构造：输入的记录集对象已经包含连接，因此不能再设定连接对象
     */
    CAdoRecordSet(const _RecordsetPtr& ptr);
    /**
     *@同上，赋值函数，将一个已经打开的记录集对象付给当前对象
     */
    CAdoRecordSet& operator = (const _RecordsetPtr& ptr);

    virtual ~CAdoRecordSet();

    /**
     *@打开记录集，当打开的记录集为空时，也会返回TRUE
     */
    BOOL Open(LPCTSTR strSQL, long lOption = adCmdText, 
                CursorTypeEnum CursorType = adOpenStatic, 
                LockTypeEnum LockType = adLockOptimistic,
                CursorLocationEnum cursorLocation = adUseClient);

    /**
     *@OpenBySQLText 以select, update等常见sql命令打开记录集
     *@OpenByTable 打开某个表，并获取表中所有记录
     */
    BOOL OpenBySQLText(LPCTSTR strSQL, CursorTypeEnum CursorType = adOpenStatic, 
        LockTypeEnum LockType = adLockOptimistic,
        CursorLocationEnum cursorLocation = adUseClient);

    BOOL OpenByTable(LPCTSTR strTableName, CursorTypeEnum CursorType = adOpenStatic, 
        LockTypeEnum LockType = adLockOptimistic,
        CursorLocationEnum cursorLocation = adUseClient);

    /**
     *@设置ado连接对象。
     *@Remarks: 对于已经打开的recordset，其Connection对象属性为 只读，尝试
     * 修改该属性无效
     */
    void SetAdoConnection(const _ConnectionPtr& pConnection);
    void SetAdoConnection(CAdoConnection* pConnection);
    void SetAdoConnection(LPCTSTR lpszConnStr);

    _ConnectionPtr GetActiveConnection();
    CAdoConnPtr GetAdoConnectionPtr();

    CAdoCmdPtr GetAdoCmdPtr();

    /**
     *@判断与当前RecordSet相关的连接对象是否已打开
     */
    BOOL IsConnectionOpen();

    BOOL Close();

    CString GetLastError();
//属性 ------------------------------------------------	
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

    // 只有CursorLocation为adUseClient时，才支持排序操作; 输入空字符串取消当前排序
    // 用法："Field1 ASC, Field2 DESC"
    void SetSort(const CString& strSort);

//记录集更新操作 --------------------------------------
public:
	BOOL AddNew();
	BOOL Update();
	BOOL UpdateBatch();

	BOOL CancelUpdate();
	BOOL CancelBatch(AffectEnum AffectRecords = adAffectAll);
	BOOL Delete(AffectEnum AffectRecords = adAffectCurrent);

//记录集导航操作 --------------------------------------
public:
	BOOL MoveFirst();
	BOOL MovePrevious();
	BOOL MoveNext();
	BOOL MoveLast();
	BOOL Move(long lRecords, long Start = adBookmarkCurrent);
	
//其他方法 --------------------------------------------
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
	// 删除记录
	BOOL DeleteCurrentRecord(void);
	BOOL DeleteAllRecord(void);	

protected:
    _RecordsetPtr		m_pRecordset;

//成员变量 --------------------------------------------
protected:
	BOOL Release();
};

/**
 * @Brief: base class for ado command operations. 
 *  应该开发应使用该类的派生类
 */
class AFX_EXT_CLASS CAdoCommand
{
public:
     /**
     *@指定一个连接对象给Command
     */
    void SetAdoConnection(CAdoConnection* pConnnection);
    void SetAdoConnection(const _ConnectionPtr& pConnnection);
    /**
     *@指定一个连接字符串给Command对象。Command对象会使用该连接字符串自动打开新连接
     *"[Provider=MSDASQL;] { DSN=name | FileDSN=filename } ; [DATABASE=database;] UID=user; PWD=password"
     */
    void SetAdoConnection(LPCTSTR lpszConnStr);

    CAdoConnPtr GetAdoConnectionPtr();
    _ConnectionPtr GetActiveConnection();

    /**
     *@判断Command对象的连接对象状态是否打开
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
     *@默认构造，未指定连接对象；在执行前，需要指定连接对象
     */
    CAdoCommand();
    /**
     *@带连接对象的构造
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
     *@设置要执行的存储过程名称
     */
    void SetStoredProcName(LPCTSTR lpszStoredProcName);

    BOOL RefreshParams();

    /**
     *@设置存储过程参数
     */
    void SetParam(LPCTSTR lpszParamName, int val);
    void SetParam(LPCTSTR lpszParamName, LPCTSTR val);

    CAdoRecSetPtr Execute();
    BOOL Execute(CAdoRecSetPtr& retRSOut);

    /**
     *@不反悔记录集的调用：可用于获取输出参数，返回值
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
