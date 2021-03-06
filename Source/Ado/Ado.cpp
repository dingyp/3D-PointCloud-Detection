// Ado.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include <cassert>
#include <cmath>
#include "ADO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/*####################################################################
	Filename: 	ado.cpp
	----------------------------------------------------
	Remarks:	...
	----------------------------------------------------

  ####################################################################*/




/*####################################################################
			------------------------------------------------
						CAdoConnection class
			------------------------------------------------
  ####################################################################*/

CAdoConnection::CAdoConnection()
: m_pConnection(NULL)
{
	///创建 Connection 对象---------------------------
	m_pConnection.CreateInstance("ADODB.Connection");
	assert(m_pConnection != NULL);
}

CAdoConnection::CAdoConnection(const _ConnectionPtr& ptr)
{
    ASSERT(ptr);
    m_pConnection = ptr;
}

CAdoConnection& CAdoConnection::operator = (const _ConnectionPtr& ptr)
{
    ASSERT(ptr);
    m_pConnection = ptr;
    return *this;
}

CAdoConnection::~CAdoConnection()
{
	//if (m_pConnection != NULL)
	//{
	//	Release();
	//}
}

/*===================================================================
	Name:		连接到数据源.
	-----------------------------------------------------
	Params:		strConnect: 连接字符串,包含连接信息.
				lOptions:	可选。决定该方法是在连接建立之后(异步)还是连接
						建立之前(同步)返回。可以是如下某个常量：
		常量					说明 
		adConnectUnspecified	(默认)同步方式打开连接。 
		adAsyncConnect			异步方式打开连接。Ado用 ConnectComplete 事
							件来通知何时完成连接。 
=====================================================================*/
BOOL CAdoConnection::Connect(LPCTSTR strConnect, long lOptions)
{
	assert(m_pConnection != NULL);
	try
	{
		// 连接数据库---------------------------------------------
		return SUCCEEDED(m_pConnection->Open(strConnect, "", "", lOptions));
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
		TRACE(_T(":( Connect Database Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	catch (...)
	{
		TRACE(_T(":( :"));
	}
	return FALSE;
}

BOOL CAdoConnection::Connect(const CString& strDSN, 
                             const CString& strID, 
                             const CString& strPW, 
                             long lOptions /* = adConnectUnspecified */)
{
    assert(m_pConnection != NULL);
    try
    {
        // 连接数据库---------------------------------------------
        return SUCCEEDED(m_pConnection->Open(strDSN.GetString(), strID.GetString(), strPW.GetString(), lOptions));
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Connect Database Error: %s\n"), e.ErrorMessage());
        return FALSE;
    } 
    catch (...)
    {
        TRACE(_T(":( :"));
    }
    return FALSE;
}

/*===================================================================
    Name: 执行命令或存储过程

    lOptions:
        adCmdUnspecified	= -1,
        adCmdUnknown	= 0x8,
        adCmdText	= 0x1,
        adCmdTable = 0x02,
        adCmdStoredProc	= 0x4,
=====================================================================*/

CAdoRecSetPtr CAdoConnection::Execute(LPCTSTR lpszCmdText, 
                             long* lpRecsAffected /* = NULL */, 
                             long lOptions /* = adCmdText */)
{
    CAdoRecSetPtr pNewRecordSet;

    VARIANT vRecs;
    ASSERT (lpszCmdText != NULL);

    if ( lpRecsAffected != NULL )
        vRecs.vt = VT_I4;
    else
        vRecs.vt = VT_EMPTY;

    try
    {
        _RecordsetPtr rs = m_pConnection->Execute(lpszCmdText, &vRecs, lOptions);
        pNewRecordSet.reset(new CAdoRecordSet(rs));
        return pNewRecordSet;
    }
    catch(_com_error e)
    {
		//m_pConnection->Close();
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Close Database Error: %s\n"), e.ErrorMessage());
        return pNewRecordSet;
    }

    return pNewRecordSet;
}

CAdoRecSetPtr CAdoConnection::ExecuteSQL(LPCTSTR lpszSQL, 
                                          long* lpRecsAffected /* = NULL */)
{
    return Execute(lpszSQL, lpRecsAffected, adCmdText);
}

CAdoRecSetPtr CAdoConnection::ExecuteQueryTable(LPCTSTR lpszTableName, 
                                          long* lpRecsAffected /* = NULL */)
{
    return Execute(lpszTableName, lpRecsAffected, adCmdTable);
}

/*===================================================================
	Name:	关闭与数据源的连接.
	-----------------------------------------------------
	Remarks: 使用 Close 方法可关闭 Connection 对象以便释放所有关联的系统资
		源。关闭对象并非将它从内存中删除，可以更改它的属性设置并且在此后再
		次打开。要将对象从内存中完全删除，需调用Release方法。
=====================================================================*/
BOOL CAdoConnection::Disconnect()
{
	try
	{
		if (m_pConnection != NULL && m_pConnection->GetState() == adStateOpen) 
		{
			if (SUCCEEDED(m_pConnection->Close()))
			{
				return TRUE;
			}
		}
	}
	catch(_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Close Database Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return FALSE;
}

/*===================================================================
	Name:	关闭连接并释放对象.
	-----------------------------------------------------
	Remarks: 关闭连接并释放connection对象，这样基本上从内容中完全清除
		了connection对象。
=====================================================================*/
BOOL CAdoConnection::Release()
{
	try
	{
		Disconnect();
		return TRUE;
	}
	catch(_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Release method Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return TRUE;
}

/*========================================================================
	Remarks:	请考参 CAdoRecordSet 类 Cancel 方法.
==========================================================================*/
BOOL CAdoConnection::Cancel()
{
	assert(m_pConnection != NULL);
	try
	{
		return m_pConnection->Cancel();
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Cancel method Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return FALSE;
}

_ConnectionPtr CAdoConnection::GetConnection()
{
	return m_pConnection;
}

CString CAdoConnection::GetConnectionStr()
{
    try
    {
        return m_pConnection->GetConnectionString().GetBSTR();
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetConnectionStr Error: %s\n"), e.ErrorMessage());
        return NULL;
    } 
    return NULL;
}

/*===================================================================
	Name:	取得最后发生的错误信息.
	-----------------------------------------------------
	Remarks: 任何涉及 ADO 对象的操作都可以产生一个或多个提供者错误。产生错
		误时,可以将一个或多个 Error 对象置于 Connection 对象的 Errors 集合
		中。其他 ADO 操作产生错误时，将清空 Errors 集合，并且将新的 Error 
		对象置于 Errors 集合中。 
			每个 Error 对象代表特定的提供者错误，而不是 ADO 错误。ADO 错误
		被记载在运行时的异常处理机制中。
			没有产生错误的 ADO 操作对 Errors 集合没有影响。使用 Clear 方式
		可手工清除 Errors 集合。
			Errors 集合中的 Error 对象集合描述响应单个语句时产生的所有错误。
		列举 Errors 集合中指定错误可使错误处理例程更精确地确定产生错误的原
		因及错误来源，并执行适当还原步骤。
			某些属性和方法将返回作为 Errors 集合中的 Error 对象显示的警告，
		但不会中止程序的执行。在调用 Recordset 对象上的 Resync、UpdateBatch
		或 CancelBatch 方法，Connection 的 Open 方法或者设置 Recordset 对象
		上的 Filter 属性前，请调用 Errors 集合上的 Clear 方法。这样您就可以
		阅读 Errors 集合的 Count 属性以测试返回的警告。
=====================================================================*/
CString CAdoConnection::GetLastError()
{
	//assert(m_pConnection != NULL);
	CString strErrors = _T("");
	try
	{
		if (m_pConnection != NULL)
		{
			ErrorsPtr pErrors = m_pConnection->Errors;
			CString strError;
			for (long n = 0; n < pErrors->Count; n++)
			{
				ErrorPtr pError = pErrors->GetItem(n);
				strError.Format(_T("Description: %s\r\n\r\nState: %s, Native: %d, Source: %s\r\n"),
								(LPCTSTR)pError->Description,
								(LPCTSTR)pError->SQLState,
										 pError->NativeError,
								(LPCTSTR)pError->Source);
				strErrors += strError;
			}
		}
		return strErrors;
	}
	catch(_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetLastError Error: %s\n"), e.ErrorMessage());
		return strErrors;
	} 
	return strErrors;
}

ErrorsPtr CAdoConnection::GetErrors()
{
	assert(m_pConnection != NULL);
	try
	{
		if (m_pConnection != NULL)
		{
			return m_pConnection->Errors;
		}
		return NULL;
	}
	catch(_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetLastError Error: %s\n"), e.ErrorMessage());
		return NULL;
	} 
	return NULL;
}

//检查当前连接是否有效，如超时或者网络断开，服务器异常等，会导致网络断开
BOOL CAdoConnection::VerifyConnection()
{
	static CTime tLast = CTime::GetCurrentTime();

	CTime t1 = CTime::GetCurrentTime();
	CTimeSpan t= t1- tLast;

	//为了避免多次检查，提高效率, 增加了延时处理
	const int nDelaySeconds =3;

	BOOL bConnection = FALSE;
	if(t.GetSeconds()<=nDelaySeconds)
	{
		return IsOpen();
	}
	else
	{	
		tLast = t1;
		try
		{
			if(m_pConnection != NULL)
			{
				long dwState =  m_pConnection->GetState();
				bConnection = (dwState & adStateOpen);
				if(bConnection)
				{
					//是否有更合适或者开销更小的命令？
					m_pConnection->BeginTrans();
					m_pConnection->RollbackTrans();
				}
			}
		}
		catch (_com_error e)
		{
			bConnection = FALSE;
			m_pConnection->Close();   //update connection state 
			//ADOLOG_ERROR(GetLastError().GetString());
			TRACE(_T(":( VerifyConnection method Error: %s\n"), e.ErrorMessage());
		} 
	}
	return bConnection;
}


BOOL CAdoConnection::IsOpen()
{
	try
	{
		long dwState =  m_pConnection->GetState();
		return (m_pConnection != NULL && (dwState & adStateOpen));
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( IsOpen method Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return FALSE;
}

void CAdoConnection::SetCursorLocateClient(BOOL bUseClient /* = TRUE */)
{
    if (m_pConnection)
    {
        try
        {
            m_pConnection->PutCursorLocation(bUseClient ? adUseClient : adUseServer);
        }
        catch (_com_error e)
        {
            ADOLOG_ERROR(GetLastError().GetString());
            TRACE(_T(":( SetCursorLocateClient Error: %s\n"), e.ErrorMessage());
			//m_pConnection->Close();
        } 
    }
}

void CAdoConnection::SetConnectTimeOut(long lTime)
{
	assert(m_pConnection != NULL);
	try
	{
		m_pConnection->PutConnectionTimeout(lTime);
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( SetConnectTimeOut Error: %s\n"), e.ErrorMessage());
	} 
}

void CAdoConnection::SetCmdTimeout(long lTime)
{
    assert(m_pConnection != NULL);
    try
    {
        m_pConnection->PutCommandTimeout(lTime);
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( SetCmdTimeout Error: %s\n"), e.ErrorMessage());
    } 
}


/*========================================================================
    Name:		从数据源获取数据库信息.
	-----------------------------------------------------
	Params:		QueryType  所要运行的模式查询类型,可以为下列任意常量.
	adSchemaAsserts				CONSTRAINT_CATALOG
								CONSTRAINT_SCHEMA
								CONSTRAINT_NAME 
	adSchemaCatalogs			CATALOG_NAME 
	adSchemaCharacterSets		CHARACTER_SET_CATALOG
								CHARACTER_SET_SCHEMA
								CHARACTER_SET_NAME 
	adSchemaCheckConstraints	CONSTRAINT_CATALOG
								CONSTRAINT_SCHEMA
								CONSTRAINT_NAME 
	adSchemaCollations			 COLLATION_CATALOG
								COLLATION_SCHEMA
								COLLATION_NAME 
	adSchemaColumnDomainUsage	DOMAIN_CATALOG
								DOMAIN_SCHEMA
								DOMAIN_NAME
								COLUMN_NAME 
	adSchemaColumnPrivileges	TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
								COLUMN_NAME
								GRANTOR
								GRANTEE 
	adSchemaColumns				TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
								COLUMN_NAME 
	adSchemaConstraintColumnUsage TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
								COLUMN_NAME 
	adSchemaConstraintTableUsage TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME 
	adSchemaForeignKeys			PK_TABLE_CATALOG
								PK_TABLE_SCHEMA
								PK_TABLE_NAME
								FK_TABLE_CATALOG
								FK_TABLE_SCHEMA
								FK_TABLE_NAME 
	adSchemaIndexes				TABLE_CATALOG
								TABLE_SCHEMA
								INDEX_NAME
								TYPE
								TABLE_NAME 
	adSchemaKeyColumnUsage		CONSTRAINT_CATALOG
								CONSTRAINT_SCHEMA
								CONSTRAINT_NAME
								TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
								COLUMN_NAME 
	adSchemaPrimaryKeys			PK_TABLE_CATALOG
								PK_TABLE_SCHEMA
								PK_TABLE_NAME 
	adSchemaProcedureColumns	PROCEDURE_CATALOG
								PROCEDURE_SCHEMA
								PROCEDURE_NAME
								COLUMN_NAME 
	adSchemaProcedureParameters PROCEDURE_CATALOG
								PROCEDURE_SCHEMA
								PROCEDURE_NAME
								PARAMTER_NAME 
	adSchemaProcedures			PROCEDURE_CATALOG
								PROCEDURE_SCHEMA
								PROCEDURE_NAME
								PROCEDURE_TYPE 
	adSchemaProviderSpecific	 参见说明 
	adSchemaProviderTypes		DATA_TYPE
								BEST_MATCH 
	adSchemaReferentialConstraints CONSTRAINT_CATALOG
								CONSTRAINT_SCHEMA
								CONSTRAINT_NAME 
	adSchemaSchemata			CATALOG_NAME
								SCHEMA_NAME
								SCHEMA_OWNER 
	adSchemaSQLLanguages		<无> 
	adSchemaStatistics			TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME 
	adSchemaTableConstraints	CONSTRAINT_CATALOG
								CONSTRAINT_SCHEMA
								CONSTRAINT_NAME
								TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
								CONSTRAINT_TYPE 
	adSchemaTablePrivileges		TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
								GRANTOR
								GRANTEE 
	adSchemaTables				TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
								TABLE_TYPE 
	adSchemaTranslations		TRANSLATION_CATALOG
								TRANSLATION_SCHEMA
								TRANSLATION_NAME 
	adSchemaUsagePrivileges		OBJECT_CATALOG
								OBJECT_SCHEMA
								OBJECT_NAME
								OBJECT_TYPE
								GRANTOR
								GRANTEE 
	adSchemaViewColumnUsage		VIEW_CATALOG
								VIEW_SCHEMA
								VIEW_NAME 
	adSchemaViewTableUsage		VIEW_CATALOG
								VIEW_SCHEMA
								VIEW_NAME 
	adSchemaViews				TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME 
	-----------------------------------------------------
	returns:	返回包含数据库信息的 Recordset 对象. Recordset 将以只读、静态
			游标打开.
	-----------------------------------------------------
	Remarks:	OpenSchema方法返回与数据源有关的信息，例如关于服务器上的表
			以及表中的列等信息, 上述数据仅供参考, 视具体的数据源可能会有不同。
==========================================================================*/
_RecordsetPtr CAdoConnection::OpenSchema(SchemaEnum QueryType)
{
	assert(m_pConnection != NULL);
	try
	{
		return m_pConnection->OpenSchema(QueryType, vtMissing, vtMissing);
	}
	catch(_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":(OpenSchema Error: %s\n"), e.ErrorMessage());
		return NULL;
	} 
	return NULL;
}

/*########################################################################
			  ------------------------------------------------
									事务处理
			  ------------------------------------------------
	BeginTrans		- 开始新事务。
	CommitTrans		- 保存任何更改并结束当前事务。它也可能启动新事务。
	RollbackTrans	- 取消当前事务中所作的任何更改并结束事务。它也可能启动
					新事务。

		一旦调用了 BeginTrans 方法，在调用 CommitTrans 或 RollbackTrans 结
	束事务之前，数据库将不再立即提交所作的任何更改。
		对于支持嵌套事务的数据库来说，在已打开的事务中调用 BeginTrans 方法
	将开始新的嵌套事务。返回值将指示嵌套层次：返回值为 1 表示已打开顶层事务
	(即事务不被另一个事务所嵌套)，返回值为 2 表示已打开第二层事务(嵌套
	在顶层事务中的事务)，依次类推。调用 CommitTrans 或 RollbackTrans 只影
	响最新打开的事务；在处理任何更高层事务之前必须关闭或回卷当前事务。
		调用 CommitTrans 方法将保存连接上打开的事务中所做的更改并结束事务。
	调用 RollbackTrans方法还原打开事务中所做的更改并结束事务。在未打开事务
	时调用其中任何一种方法都将引发错误。
		取决于 Connection 对象的 Attributes 属性，调用 CommitTrans 或 
	RollbackTrans 方法都可以自动启动新事务。如果 Attributes 属性设置为 
	adXactCommitRetaining，数据库在 CommitTrans 调用后会自动启动新事务。
	如果 Attributes 属性设置为 adXactAbortRetaining，数据库在调用 
	RollbackTrans 之后将自动启动新事务。
  ########################################################################*/
long CAdoConnection::BeginTrans()
{
	assert(m_pConnection != NULL);
	try
	{
		return m_pConnection->BeginTrans();
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( BeginTrans Error: %s\n"), e.ErrorMessage());
		return -1;
	} 
	return -1;
}

BOOL CAdoConnection::CommitTrans()
{
	assert(m_pConnection != NULL);
	try
	{
		return SUCCEEDED(m_pConnection->CommitTrans());
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( CommitTrans Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return FALSE;
}

BOOL CAdoConnection::RollbackTrans()
{
	assert(m_pConnection != NULL);
	try
	{
		return SUCCEEDED(m_pConnection->RollbackTrans());
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":(RollbackTrans Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return FALSE;
}
BOOL CAdoConnection::ClearTbl(TCHAR* TblName)
{
	assert(m_pConnection != NULL);
	try
	{
        CString strSQL;
		strSQL.Format(_T("TRUNCATE TABLE %s"), TblName);
		return SUCCEEDED(m_pConnection->Execute(_bstr_t(strSQL.GetString()), 
            NULL, adExecuteNoRecords));
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":(ClearTbl Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return FALSE;
}
/*########################################################################
			------------------------------------------------
					 CAdoRecordSet class 构造函数
			------------------------------------------------
  ########################################################################*/

CAdoRecordSet::CAdoRecordSet()
{
	m_pRecordset.CreateInstance("ADODB.Recordset");
	assert(m_pRecordset != NULL);
}

CAdoRecordSet::CAdoRecordSet(const _RecordsetPtr& ptr)
{
    ASSERT(ptr);
    m_pRecordset = ptr;
}

CAdoRecordSet& CAdoRecordSet::operator = (const _RecordsetPtr& ptr)
{
    ASSERT (ptr);
    m_pRecordset = ptr;
    return *this;
}

CAdoRecordSet::CAdoRecordSet(const _ConnectionPtr& pConnection)
{
    ASSERT(pConnection && pConnection->GetState() & adStateOpen);

	m_pRecordset->PutActiveConnection( _variant_t( (IDispatch*)pConnection ) );
	m_pRecordset.CreateInstance("ADODB.Recordset");
	assert(m_pRecordset != NULL);
}

CAdoRecordSet::~CAdoRecordSet()
{
	Release();
}

/*========================================================================
	Params:		
		- strSQL:		计算 Command 对象的变量名,SQL 语句,表名,存储过程调
					用或持久 Recordset 文件名.
		- CursorType:   可选.CursorTypeEnum 值, 确定打开Recordset时应
					该使用的游标类型.可为下列常量之一.
		常量				说明 
		-----------------------------------------------
		adOpenForwardOnly	打开仅向前类型游标. 
		adOpenKeyset		打开键集类型游标. 
		adOpenDynamic		打开动态类型游标. 
		adOpenStatic		打开静态类型游标. 
		-----------------------------------------------
		- LockType:     可选,确定打开Recordset时应该使用的锁定类型(并发)的
					LockTypeEnum值,可为下列常量之一.
		常量				说明 
		-----------------------------------------------
		adLockReadOnly		只读 — 不能改变数据. 
		adLockPessimistic	保守式锁定 - 通常通过在编辑时立即锁定数据源的记录. 
		adLockOptimistic	开放式锁定 - 只在调用 Update 方法时才锁定记录. 
		adLockBatchOptimistic 开放式批更新 - 用于批更新模式(与立即更新模式
						相对). 
		-----------------------------------------------
		- lOption		可选. 长整型值, 用于指示 strSQL 参数的类型可为下列
					常量之一.
		常量				说明 
		-------------------------------------------------
		adCmdText			指示strSQL为命令文本, 即普通的SQL语句. 
		adCmdTable			指示ADO生成SQL查询以便从在strSQL中命名的表中返
						回所有行. 
		adCmdTableDirect	指示所作的更改在strSQL中命名的表中返回所有行. 
		adCmdStoredProc		指示strSQL为存储过程. 
		adCmdUnknown		指示strSQL参数中的命令类型为未知. 
		adCmdFile			指示应从在strSQL中命名的文件中恢复保留(保存的)
						Recordset. 
		adAsyncExecute		指示应异步执行strSQL. 
		adAsyncFetch		指示在提取 Initial Fetch Size 属性中指定的初始
						数量后,应该异步提取所有剩余的行.如果所需的行尚未提
						取,主要的线程将被堵塞直到行重新可用. 
		adAsyncFetchNonBlocking 指示主要线程在提取期间从未堵塞. 如果所请求
						的行尚未提取,当前行自动移到文件末尾. 
==========================================================================*/
BOOL CAdoRecordSet::Open(LPCTSTR strSQL, 
                            long lOption,
                            CursorTypeEnum CursorType,
                            LockTypeEnum LockType,
                            CursorLocationEnum cursorLocation)
{
	if (!m_pRecordset)
        return FALSE;

	try
	{
		Close();
        m_pRecordset->PutCursorLocation(cursorLocation);
		HRESULT hr = m_pRecordset->Open(_bstr_t(strSQL),
            _variant_t((IDispatch*)m_pRecordset->GetActiveConnection(), true),
							CursorType, LockType, lOption);
		
		return SUCCEEDED(hr);
	}
	catch (_com_error e)
	{
		ADOLOG_ERROR(strSQL);
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T("Open Error : %s\n"), e.ErrorMessage());
		return FALSE;
	}
    return FALSE;
}

BOOL CAdoRecordSet::OpenBySQLText(LPCTSTR strSQL, 
                                  CursorTypeEnum CursorType /* = adOpenStatic */, 
                                  LockTypeEnum LockType /* = adLockOptimistic */, 
                                  CursorLocationEnum cursorLocation /* = adUseClient */)
{
    return Open(strSQL, adCmdText, CursorType, LockType, cursorLocation);
}

BOOL CAdoRecordSet::OpenByTable(LPCTSTR strTableName, 
                                CursorTypeEnum CursorType /* = adOpenStatic */, 
                                LockTypeEnum LockType /* = adLockOptimistic */, 
                                CursorLocationEnum cursorLocation /* = adUseClient */)
{
    return Open(strTableName, adCmdTable, CursorType, LockType, cursorLocation);
}

/*===================================================================
	Name:		关闭打开的对象及任何相关对象.
	-----------------------------------------------------
	Remarks:	使用Close方法可关闭Recordset对象以便释放所有关联的系统资源.
		关闭对象并非将它从内存中删除, 可以更改它的属性设置并且在此后再次打
		开.要将对象从内存中完全删除, 可将对象变量设置为 Nothing.
		    使用Close方法关闭Recordset对象的同时, 将释放关联的数据和可能已
		经通过该特定Recordset对象对数据进行的独立访问.随后可调用Open方法重
		新打开具有相同属性或已修改属性的Recordset.在Recordset对象关闭后,调
		用任何需要活动游标的方法将产生错误.
			如果正在立即更新模式下进行编辑,调用Close方法将产生错误, 应首先
		调用Update或CancelUpdat 方法.如果在批更新期间关闭Recordset对象, 则
		自上次UpdateBatch调用以来所做的修改将全部丢失.
			如果使用Clone方法创建已打开的Recordset对象的副本,关闭原始Recordset
		或其副本将不影响任何其他副本.
			要将对象从内存中完全删除，需调用Release方法。
=====================================================================*/
BOOL CAdoRecordSet::Close()
{
	try
	{
		if (m_pRecordset != NULL && m_pRecordset->State != adStateClosed)
		{
			return SUCCEEDED(m_pRecordset->Close());
		}
		return TRUE;
	}
	catch (const _com_error& e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T("Close Dateset Error: %s\n"), e.ErrorMessage());
		return FALSE;
	}

	return TRUE;
}

CString CAdoRecordSet::GetLastError()
{
    CAdoConnPtr connPtr = GetAdoConnectionPtr();
    if (connPtr)
    {
        return connPtr->GetLastError().GetString();
    }
    else
    {
        CAdoCmdPtr cmdPtr = GetAdoCmdPtr();
        if (cmdPtr)
        {
            return cmdPtr->GetLastError();
        }
    }
    return _T("");
}

/*===================================================================
	Name:	关闭连接并释放对象.
	-----------------------------------------------------
	Remarks: 关闭连接并释放CAdoRecordSet对象，这样基本上从内容中完全清除
		了CAdoRecordSet对象。
=====================================================================*/
BOOL CAdoRecordSet::Release()
{
	try
	{
		if (m_pRecordset != NULL)
		{
			Close();
		}
		return TRUE;
	}
	catch (const _com_error& e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T("Release Dataset Error: %s\n"), e.ErrorMessage());
		return FALSE;
	}
	return TRUE;
}

/*########################################################################
			  ------------------------------------------------
							   记录集更新操作
			  ------------------------------------------------
  ########################################################################*/

/*===================================================================
	Remarks:	添加新的纪录.先AddNew()声明, 然后用PutCollect()设置每
				列的值, 最后用Update()更新数据库.
=====================================================================*/
BOOL CAdoRecordSet::AddNew()
{
	assert(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->AddNew());
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( AddNew Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return	FALSE;
}

/*===================================================================
	Remarks:	在调用AddNew()等方法后，调用此方法完成更新或修改.
=====================================================================*/
BOOL CAdoRecordSet::Update()
{
	assert(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->Update());
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Update Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return	FALSE;
}

BOOL CAdoRecordSet::UpdateBatch()
{
	assert(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->UpdateBatch(adAffectAll));
		}
	}
	catch (_com_error e)
	{
		ADOLOG_ERROR(GetLastError().GetString());
		TRACE(_T(":( UpdateBatch Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return	FALSE;
}


/*========================================================================
	Name:		取消在调用 Update 方法前对当前记录或新记录所作的任何更改.
	-----------------------------------------------------
	Remarks:	使用 CancelUpdate 方法可取消对当前记录所作的任何更改或
	放弃新添加的记录.在调用 Update 方法后将无法撤消对当前记录或新记录
	所做的更改,除非更改是可以用 RollbackTrans 方法回卷的事务的一部分,
	或者是可以用 CancelBatch 方法取消的批更新的一部分.
	如果在调用 CancelUpdate 方法时添加新记录,则调用 AddNew 之前的当前
	记录将再次成为当前记录.
	如果尚未更改当前记录或添加新记录,调用 CancelUpdate 方法将产生错误.
==========================================================================*/
BOOL CAdoRecordSet::CancelUpdate()
{
	assert(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->CancelUpdate());
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( CancelUpdate Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return FALSE;
}

/*========================================================================
	Name:		取消挂起的批更新.
	-----------------------------------------------------
	Params:		AffectRecords   可选的 AffectEnum 值,决定CancelBatch 
		方法所影响记录的数目,可为下列常量之一：
		常量			说明 
		-------------------------------------------------
		AdAffectCurrent 仅取消当前记录的挂起更新. 
		AdAffectGroup	对满足当前 Filter 属性设置的记录取消挂起更新.
						使用该选项时,必须将 Filter 属性设置为合法的预
						定义常量之一. 
		AdAffectAll		默认值.取消 Recordset 对象中所有记录的挂起更
						新,包括由当前 Filter 属性设置所隐藏的任何记录. 
==========================================================================*/
BOOL CAdoRecordSet::CancelBatch(AffectEnum AffectRecords)
{
	assert(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->CancelBatch(AffectRecords));
		}
	}
	catch(_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( CancelBatch Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return FALSE;
}

/*========================================================================
	Params:		AffectRecords:  AffectEnum值, 确定Delete方法所影响的记录数
		目,该值可以是下列常量之一.
		常量				说明 
		-------------------------------------------------
		AdAffectCurrent		默认.仅删除当前记录. 
		AdAffectGroup		删除满足当前Filter属性设置的记录.要使用该选项,
						必须将 Filter 属性设置为有效的预定义常量之一. 
		adAffectAll			删除所有记录. 
		adAffectAllChapters 删除所有子集记录. 
==========================================================================*/
BOOL CAdoRecordSet::Delete(AffectEnum AffectRecords)
{
	assert(m_pRecordset != NULL);
	try
	{
		if(SUCCEEDED(m_pRecordset->Delete(AffectRecords)))
			return SUCCEEDED(m_pRecordset->Update());		
	}
	catch(_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Delete Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return	FALSE;
}

/*########################################################################
			  ------------------------------------------------
								记录集导航操作
			  ------------------------------------------------
    Remarks:	
		使用MoveFirst方法将当前记录位置移动到Recordse中的第一个记录.
		使用MoveLast方法将当前记录位置移动到Recordset中的最后一个记录.
	Recordset对象必须支持书签或向后光标移动;否则调用该方法将产生错误.

		使用MoveNext方法将当前记录向前移动一个记录(向Recordset的底部).如果
	最后一个记录是当前记录并且调用MoveNext方法,则ADO将当前记录设置到
	Recordset(EOF为True)的尾记录之后.当EOF属性已经为True时试图向前移动
	将产生错误.
		使用MovePrevious方法将当前记录位置向后移动一个记录(向记录集的顶部).
	Recordset 对象必须支持书签或向后游标移动;否则方法调用将产生错误.
	如果首记录是当前记录并且调用MovePrevious方法,则ADO将当前记录设置在
	Recordset(BOF为True)的首记录之前.而BOF属性为True时向后移动将产生错
	误. 如果Recordset对象不支持书签或向后游标移动,则MovePrevious方法将
	产生错误.
		如果记录集是仅向前的,但是用户希望支持向前和向后滚动,则可以使用
	CacheSize 属性创建记录缓存,通过 Move方法支持向后游标移动.由于
	缓存记录是加载到内存中的,所以应避免不必要地缓存太多记录.可以调用
	仅向前 Recordset 对象的 MoveFirst 方法;这样做可使提供者重新执行生
	成 Recordset 对象的命令.
  ########################################################################*/

BOOL CAdoRecordSet::MoveFirst()
{
	assert(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->MoveFirst());
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( MoveFirst Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return	FALSE;
}

BOOL CAdoRecordSet::MoveLast()
{
	assert(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->MoveLast());
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( MoveLast Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return	FALSE;
}

BOOL CAdoRecordSet::MovePrevious()
{
	assert(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->MovePrevious());
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( MovePrevious Error: %s\n"), e.ErrorMessage());
		return FALSE;
	}
	return FALSE;	
}

BOOL CAdoRecordSet::MoveNext()
{
	assert(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->MoveNext());
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( MoveNext Error: %s\n"), e.ErrorMessage());
		return FALSE;
	}
	return FALSE;
}

/*========================================================================
	Name:		移动 Recordset 对象中当前记录的位置。
    ----------------------------------------------------------
	Params:		
		- lRecords    带符号长整型表达式，指定当前记录位置移动的记录数。
		- Start    可选，字符串或变体型，用于计算书签。也可为下列 BookmarkEnum 值之一：
		常量				说明 
		adBookmarkCurrent	默认。从当前记录开始。 
		adBookmarkFirst		从首记录开始。 
		AdBookmarkLast		从尾记录开始。 
==========================================================================*/
BOOL CAdoRecordSet::Move(long lRecords, long Start)
{
	assert(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->Move(lRecords, _variant_t(Start)));
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Move Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return	FALSE;
}

/*########################################################################
			  ------------------------------------------------
								  记录集属性
			  ------------------------------------------------
  ########################################################################*/

/*========================================================================
	Name:		对所有可应用对象,说明其对象状态是打开或是关闭.对执行
	异步方法的 Recordset 对象,说明当前的对象状态是连接、执行或是获取.
	-----------------------------------------------------
	returns:	返回下列常量之一的长整型值.
		常量				说明 
		----------------------------------
		adStateClosed		指示对象是关闭的. 
		adStateOpen			指示对象是打开的. 
		adStateConnecting	指示 Recordset 对象正在连接. 
		adStateExecuting	指示 Recordset 对象正在执行命令. 
		adStateFetching		指示 Recordset 对象的行正在被读取. 
	-----------------------------------------------------
	Remarks:		可以随时使用 State 属性确定指定对象的当前状态.该
	属性是只读的.Recordset 对象的 State 属性可以是组合值.例如,如果
	正在执行语句,该属性将是 adStateOpen 和 adStateExecuting 的组合值.
==========================================================================*/
long CAdoRecordSet::GetState()
{
	assert(m_pRecordset != NULL);
	long lState = 0;
	try
	{
		if (SUCCEEDED(m_pRecordset->get_State(&lState)))
		{
			return lState;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetState Error: %s\n"), e.ErrorMessage());
		return NULL;
	} 
	return NULL;
}

/*========================================================================
	Name:		指示有关批更新或其他大量操作的当前记录的状态.
	-----------------------------------------------------
	returns:	返回下列一个或多个 RecordStatusEnum 值之和.
		常量						说明 
		-------------------------------------------------
		adRecOK						成功地更新记录. 
		adRecNew					记录是新建的. 
		adRecModified				记录被修改. 
		adRecDeleted				记录被删除. 
		adRecUnmodified				记录没有修改. 
		adRecInvalid				由于书签无效,记录没有保存. 
		adRecMultipleChanges		由于影响多个记录,因此记录未被保存. 
		adRecPendingChanges			由于记录引用挂起的插入,因此未被保存. 
		adRecCanceled				由于操作被取消,未保存记录. 
		adRecCantRelease			由于现有记录锁定,没有保存新记录. 
		adRecConcurrencyViolation	由于开放式并发在使用中,记录未被保存. 
		adRecIntegrityViolation		由于用户违反完整性约束,记录未被保存. 
		adRecMaxChangesExceeded		由于存在过多挂起更改,记录未被保存. 
		adRecObjectOpen				由于与打开的储存对象冲突,记录未被保存. 
		adRecOutOfMemory			由于计算机内存不足,记录未被保存. 
		adRecPermissionDenied		由于用户没有足够的权限,记录未被保存. 
		adRecSchemaViolation		由于记录违反基本数据库的结构,因此未被保存. 
		adRecDBDeleted				记录已经从数据源中删除. 
	-----------------------------------------------------
	Remarks:	使用 Status 属性查看在批更新中被修改的记录有哪些更改被
	挂起.也可使用 Status 属性查看大量操作时失败记录的状态.例如,调用 
	Recordset 对象的 Resync、UpdateBatch 或 CancelBatch 方法,或者设置 
	Recordset 对象的 Filter 属性为书签数组.使用该属性,可检查指定记录
	为何失败并将问题解决.
==========================================================================*/
long CAdoRecordSet::GetStatus()
{
	assert(m_pRecordset != NULL);
	long lStatus = 0;
	try
	{
		if (SUCCEEDED(m_pRecordset->get_Status(&lStatus)))
		{
			return lStatus;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetStatus Error: %s\n"), e.ErrorMessage());
		return NULL;
	} 
	return NULL;
}

/*========================================================================
	Name:		获取当前记录集中记录数目
==========================================================================*/
long CAdoRecordSet::GetRecordCount()
{
	assert(m_pRecordset != NULL);
	long lcount=0;
	try
	{
		m_pRecordset->get_RecordCount(&lcount);
		return lcount;
	}
	catch(_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetRecordCount Error: %s\n"), e.ErrorMessage());
		return 0;
	} 
}

/*========================================================================
	Name:		获取当前记录集中字段数目
==========================================================================*/
long CAdoRecordSet::GetFieldsCount()
{
	assert(m_pRecordset != NULL);
	try
	{
		return GetFields()->Count;
	}
	catch(_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetFieldsCount Error: %s\n"), e.ErrorMessage());
		return -1;
	} 
}

/*========================================================================
	Name:		指针是否在在记录集头
==========================================================================*/
BOOL CAdoRecordSet::IsBOF()
{
	assert(m_pRecordset != NULL);
	try
	{
		return m_pRecordset->GetadoBOF();
	}
	catch(_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( IsBOF Error: %s\n"), e.ErrorMessage());
	} 
	return TRUE;
}

/*========================================================================
	Name:		指针是否在在记录集尾
==========================================================================*/
BOOL CAdoRecordSet::IsEOF()
{
	assert(m_pRecordset != NULL);
	try
	{
		return m_pRecordset->GetadoEOF();
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( IsEOF Error: %s\n"), e.ErrorMessage());
        CString str = GetLastError().GetString();
	}
    return TRUE;
}

void CAdoRecordSet::SetAdoConnection(const _ConnectionPtr& pConnection)
{
    ASSERT(m_pRecordset);
    try
    {
        if (!IsOpen())
        {
            ASSERT(pConnection && pConnection->GetState() & adStateOpen);
            m_pRecordset->PutActiveConnection( _variant_t( (IDispatch*)pConnection ) );
        }
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( SetAdoConnection Error: %s\n"), e.ErrorMessage());
    } 
}

void CAdoRecordSet::SetAdoConnection(CAdoConnection* pConn)
{
    ASSERT(m_pRecordset);
    try
    {
        if (!IsOpen())
        {
            ASSERT(pConn && pConn->IsOpen());
            m_pRecordset->PutActiveConnection( _variant_t( (IDispatch*)pConn->GetConnection() ) );
        }
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( SetAdoConnection Error: %s\n"), e.ErrorMessage());
    } 
}

void CAdoRecordSet::SetAdoConnection(LPCTSTR lpszConnStr)
{
    ASSERT(m_pRecordset);
    
    try
    {
        m_pRecordset->PutActiveConnection(lpszConnStr);
    }
    catch(_com_error &e) 
    {       
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( SetAdoConnection Error: %s\n"), e.ErrorMessage());
    }
}

_ConnectionPtr CAdoRecordSet::GetActiveConnection()
{
    _ConnectionPtr ptr;
    if (m_pRecordset)
    {
        try
        {
            ptr = m_pRecordset->GetActiveConnection();
        }
        catch (_com_error e)
        {
            ADOLOG_ERROR(GetLastError().GetString());
            TRACE(_T(":( GetActiveConnection Error: %s\n"), e.ErrorMessage());
        } 
    }
    
    return ptr;
}

BOOL CAdoRecordSet::IsConnectionOpen() 
{
    try
    {
        return GetActiveConnection() && GetActiveConnection()->GetState() & adStateOpen;
    }
    catch(_com_error &e) 
    {       
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( IsConnectionOpen Error: %s\n"), e.ErrorMessage());
    }
    return FALSE;
}

CAdoConnPtr CAdoRecordSet::GetAdoConnectionPtr()
{
    CAdoConnPtr ret;

    if (m_pRecordset)
    {
        try
        {
            _ConnectionPtr ptr = GetActiveConnection();
            if (ptr)
            {
                ret.reset(new CAdoConnection(ptr));
            }
        }
        catch (_com_error e)
        {
            ADOLOG_ERROR(GetLastError().GetString());
            TRACE(_T(":( GetAdoConnectionPtr Error: %s\n"), e.ErrorMessage());
        } 
    }

    return ret;
}

CAdoCmdPtr CAdoRecordSet::GetAdoCmdPtr()
{
    CAdoCmdPtr ptr;
    try
    {
        if (m_pRecordset)
        {
            IDispatchPtr cmdPtr = m_pRecordset->GetActiveCommand();
            if ( cmdPtr )
            {
                ptr.reset(new CAdoCommand(cmdPtr));
            }
        }
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetAdoCmdPtr Error: %s\n"), e.ErrorMessage());
    } 
    
    return ptr;
}

/*===================================================================
	Remarks:	Recordset 对象包括 Field 对象组成的 Fields 集合.每个 
	Field 对象对应 Recordset 集中的一列.
=====================================================================*/
Fields* CAdoRecordSet::GetFields()
{
	assert(m_pRecordset != NULL);
	FieldsPtr pFields;
	try
	{
		if (SUCCEEDED(m_pRecordset->get_Fields(&pFields)))
		{
			return pFields;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetFields Error: %s\n"), e.ErrorMessage());
		return NULL;
	} 
	return NULL;
}

/*========================================================================
	Remarks:	取得指定列字段的字段名.
==========================================================================*/
CString CAdoRecordSet::GetFieldName(long lIndex)
{
	assert(m_pRecordset != NULL);
	CString strFieldName;
	try
	{
		FieldPtr field = GetFields()->GetItem(_variant_t(lIndex));
		strFieldName = LPCTSTR(_bstr_t(field->GetName()));
		return strFieldName;
		field->GetType();
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetFieldName Error: %s\n"), e.ErrorMessage());
		return strFieldName;
	}
}

/*========================================================================
	returns:	对于 Field 对象，Attributes 属性为只读，其值可能为以下任意一个或多个 FieldAttributeEnum 值的和。
	  常量				说明 
	  adFldMayDefer			指示字段被延迟，即不从拥有整个记录的数据源检索
						字段值，仅在显式访问这些字段时才进行检索。 
	  adFldUpdatable		指示可以写入该字段。 
	  adFldUnknownUpdatable 指示提供者无法确定是否可以写入该字段。 
	  adFldFixed			指示该字段包含定长数据。 
	  adFldIsNullable		指示该字段接受 Null 值。 
	  adFldMayBeNull		指示可以从该字段读取 Null 值。 
	  adFldLong				指示该字段为长二进制字段。并指示可以使用 AppendChunk 和 GetChunk 方法。 
	  adFldRowID			指示字段包含持久的行标识符，该标识符无法被写入
						并且除了对行进行标识(如记录号、唯一标识符等)外不
						存在有意义的值。 
	  adFldRowVersion		指示该字段包含用来跟踪更新的某种时间或日期标记。 
	  adFldCacheDeferred	指示提供者缓存了字段值，并已完成随后对缓存的读取。 
    ----------------------------------------------------------
	Remarks:	取得 Field 对象一项或多项属性。
==========================================================================*/
long CAdoRecordSet::GetFieldAttributes(long lIndex)
{
	assert(m_pRecordset != NULL);
	long lAttributes;
	try
	{
		FieldPtr field = GetFields()->GetItem(_variant_t(lIndex));
		field->get_Attributes(&lAttributes);
		return lAttributes;
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetFieldAttributes Error: %s\n"), e.ErrorMessage());
		return -1;
	}
}
/*========================================================================
	Name:		指示 Field 对象所定义的长度。
    ----------------------------------------------------------
	returns:	返回某个字段定义的长度(按字节数)的长整型值。
    ----------------------------------------------------------
	Remarks:	使用 DefinedSize 属性可确定 Field 对象的数据容量。
==========================================================================*/

long CAdoRecordSet::GetFieldDefineSize(long lIndex)
{
	assert(m_pRecordset != NULL);
	try
	{
		FieldPtr field = GetFields()->GetItem(_variant_t(lIndex));
		return field->GetDefinedSize();
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetDefineSize Error: %s\n"), e.ErrorMessage());
		return -1;
	}
}

/*========================================================================
	Name:	取得字段的值的实际长度。
    ----------------------------------------------------------
	returns:	返回长整型值。某些提供者允许设置该属性以便为 BLOB 数据预留
			空间，在此情况下默认值为 0。
    ----------------------------------------------------------
	Remarks:	使用 ActualSize 属性可返回 Field 对象值的实际长度.对于所有
			字段,ActualSize 属性为只读。如果 ADO 无法确定 Field 对象值的实
			际长度，ActualSize 属性将返回 adUnknown。
				如以下范例所示，ActualSize 和  DefinedSize 属性有所不同：
			adVarChar 声明类型且最大长度为 50 个字符的 Field 对象将返回为 
			50 的 DefinedSize 属性值，但是返回的 ActualSize 属性值是当前记
			录的字段中存储的数据的长度。
==========================================================================*/
long CAdoRecordSet::GetFieldActualSize(long lIndex)
{
	assert(m_pRecordset != NULL);
	try
	{
		FieldPtr field = GetFields()->GetItem(_variant_t(lIndex));
		return field->GetActualSize();
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetFieldActualSize Error: %s\n"), e.ErrorMessage());
		return -1;
	}	
}

/*========================================================================
	returns:	返回下列值之一。相应的 OLE DB 类型标识符在下表的说明栏的括号中给出。
	  常量				说明 
	  ---------------------------------------------------------
	  adArray			与其他类型一起加入逻辑 OR 以指示该数据是那种类型的
					安全数组 (DBTYPE_ARRAY)。 
	  adBigInt			8 字节带符号的整数 (DBTYPE_I8)。 
	  adBinary			二进制值 (DBTYPE_BYTES)。 
	  adBoolean			布尔型值 (DBTYPE_BOOL)。 
	  adByRef			与其他类型一起加入逻辑 OR 以指示该数据是其他类型数
					据的指针 (DBTYPE_BYREF)。 
	  adBSTR			以空结尾的字符串 (Unicode) (DBTYPE_BSTR)。 
	  adChar			字符串值 (DBTYPE_STR)。 
	  adCurrency		货币值 (DBTYPE_CY)。货币数字的小数点位置固定、小数
					点右侧有四位数字。该值保存为 8 字节范围为10,000 的带符
					号整型值。 
	  adDate			日期值 (DBTYPE_DATE)。日期按双精度型数值来保存，数
					字全部表示从 1899 年 12 月 30 开始的日期数。小数部分是
					一天当中的片段时间。 
	  adDBDate			日期值 (yyyymmdd) (DBTYPE_DBDATE)。 
	  adDBTime			时间值 (hhmmss) (DBTYPE_DBTIME)。 
	  adDBTimeStamp		时间戳 (yyyymmddhhmmss 加 10 亿分之一的小数)(DBTYPE_DBTIMESTAMP). 
	  adDecimal			具有固定精度和范围的精确数字值 (DBTYPE_DECIMAL)。 
	  adDouble			双精度浮点值 (DBTYPE_R8)。 
	  adEmpty			未指定值 (DBTYPE_EMPTY)。 
	  adError			32 - 位错误代码 (DBTYPE_ERROR)。 
	  adGUID			全局唯一的标识符 (GUID) (DBTYPE_GUID)。 
	  adIDispatch		OLE 对象上 Idispatch 接口的指针 (DBTYPE_IDISPATCH)。 
	  adInteger			4 字节的带符号整型 (DBTYPE_I4)。 
	  adIUnknown		OLE 对象上 IUnknown 接口的指针 (DBTYPE_IUNKNOWN)。
	  adLongVarBinary	长二进制值。 
	  adLongVarChar		长字符串值。 
	  adLongVarWChar	以空结尾的长字符串值。 
	  adNumeric			具有固定精度和范围的精确数字值 (DBTYPE_NUMERIC)。 
	  adSingle			单精度浮点值 (DBTYPE_R4)。 
	  adSmallInt		2 字节带符号整型 (DBTYPE_I2)。 
	  adTinyInt			1 字节带符号整型 (DBTYPE_I1)。 
	  adUnsignedBigInt	8 字节不带符号整型 (DBTYPE_UI8)。 
	  adUnsignedInt		4 字节不带符号整型 (DBTYPE_UI4)。 
	  adUnsignedSmallInt 2 字节不带符号整型 (DBTYPE_UI2)。 
	  adUnsignedTinyInt 1 字节不带符号整型 (DBTYPE_UI1)。 
	  adUserDefined		用户定义的变量 (DBTYPE_UDT)。 
	  adVarBinary		二进制值。 
	  adVarChar			字符串值。 
	  adVariant			自动变体型 (DBTYPE_VARIANT)。 
	  adVector			与其他类型一起加入逻辑 OR 中，指示数据是 DBVECTOR 
					结构(由 OLE DB 定义)。该结构含有元素的计数和其他类型 
					(DBTYPE_VECTOR) 数据的指针。 
	  adVarWChar		以空结尾的 Unicode 字符串。 
	  adWChar			以空结尾的 Unicode 字符串 (DBTYPE_WSTR)。 
    ----------------------------------------------------------
	Remarks:	返回指定列的数据类型。
==========================================================================*/
DWORD CAdoRecordSet::GetFieldType(long lIndex)
{
	assert(m_pRecordset != NULL);
	try 
	{
		FieldPtr field = GetFields()->GetItem(_variant_t(lIndex));
		return field->GetType();
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetField Error: %s\n"), e.ErrorMessage());
		return NULL;
	}	
}

DWORD CAdoRecordSet::GetFieldType(LPCTSTR strfield)
{
	assert(m_pRecordset != NULL);
	try 
	{
		FieldPtr field = GetFields()->GetItem(strfield);
		return field->GetType();
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetField Error: %s\n"), e.ErrorMessage());
		return NULL;
	}	
}
/*========================================================================
	Name:	取得指定列的字段对象的指针.	
==========================================================================*/
FieldPtr CAdoRecordSet::GetField(long lIndex)
{
	try
	{
		return GetFields()->GetItem(_variant_t(lIndex));
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetField Error: %s\n"), e.ErrorMessage());
		return NULL;
	}
}

const _RecordsetPtr& CAdoRecordSet::GetRecordset()
{
	return m_pRecordset;
}

void CAdoRecordSet::SetFilter(const CString& strFilter)
{
    try
    {
        m_pRecordset->Filter = strFilter.GetString();
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T("SetFilter exception: %s\n"), e.ErrorMessage());
    }
}

void CAdoRecordSet::SetSort(const CString& strSort)
{
    try
    {
        m_pRecordset->put_Sort(_bstr_t(strSort.GetString()));
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T("SetSort exception: %s\n"), e.ErrorMessage());
    }
}

//CString CAdoRecordSet::GetLastError().GetString()
//{
//	return m_pConnection->GetLastError().GetString();
//}

/*========================================================================
	Name:		取消执行挂起的异步 Execute 或 Open 方法的调用.
	-----------------------------------------------------
	Remarks:	使用 Cancel 方法终止执行异步 Execute 或 Open 方法调用(即通
		过 adAsyncConnect、adAsyncExecute 或 adAsyncFetch 参数调用的方法).
		如果在试图终止的方法中没有使用 adAsyncExecute,则 Cancel 将返回运行
		时错误.
			下表显示在特定类型对象上使用 Cancel 方法时将终止的任务.
		如果对象为		将终止对该方法的上一次异步调用 
		-------------------------------------------------
		Command			Execute 
		Connection		Execute 或 Open 
		Recordset		Open 
==========================================================================*/
BOOL CAdoRecordSet::Cancel()
{
	assert(m_pRecordset != NULL);
	try
	{
		return m_pRecordset->Cancel();
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Cancel Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return FALSE;
}

_RecordsetPtr CAdoRecordSet::operator =(_RecordsetPtr &pRecordSet)
{
	Release();
	m_pRecordset = pRecordSet;
	return m_pRecordset;
}

/*########################################################################
			  ------------------------------------------------
								   其他方法
			  ------------------------------------------------
  ########################################################################*/

/*========================================================================
	Remarks:	取得当前记录中指定字段的值并转换成字符串
==========================================================================*/
BOOL CAdoRecordSet::GetValueString(CString& strValue, long lFieldIndex)
{
	assert(m_pRecordset != NULL);
	if (lFieldIndex < 0 || lFieldIndex >= GetFieldsCount())
	{
		return FALSE;
	}
	try
	{
		_variant_t value = m_pRecordset->GetCollect(_variant_t(lFieldIndex));
		strValue = Format(value);
		return TRUE;
	}
	catch (_com_error e)
	{
       // ADOLOG_ERROR(GetLastError().GetString());
		ADOLOG_ERROR(e.ErrorMessage());
        TRACE(_T(":( Access Field  Failed: %s\n"), e.ErrorMessage());
		return FALSE;
	}

	return FALSE;
}

BOOL CAdoRecordSet::GetValueString(CString &strValue, CString strFieldName)
{
	assert(m_pRecordset != NULL);
	try
	{
		_variant_t value = m_pRecordset->GetCollect(_variant_t(LPCTSTR(strFieldName)));
		strValue = Format(value);
		return TRUE;	
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":(  Access Field Failed: %s\n"), e.ErrorMessage());
		return FALSE;
	}

	return FALSE/**/;
}

BOOL CAdoRecordSet::GetValue(int& val, LPCTSTR fieldName)
{
	assert(m_pRecordset != NULL);
	CString strValue;
	if (GetValueString(strValue, fieldName))
	{
		val = _tstoi(strValue);
		return TRUE;
	}

	return FALSE;
}

/*========================================================================
	Remarks:	把_variant_t型变量格式化成CString型字符串.
==========================================================================*/
CString CAdoRecordSet::Format(_variant_t var)
{
	DATE dt;
	COleDateTime da;
	CString strValue;
	
	switch (var.vt)
	{
	case VT_BSTR://字符串
	case VT_LPSTR://字符串
	case VT_LPWSTR://字符串
		strValue = (LPCTSTR)(_bstr_t)var;
		break;
	case VT_I1://短整型                            //yk 20140918 fix mysql bool filed read error
	case VT_I2://短整型
		strValue.Format(_T("%d"), var.iVal);
		break;
	case VT_UI2://无符号短整型
		strValue.Format(_T("%d"), var.uiVal);
		break;
	case VT_VOID://
	case VT_INT://整型
	case VT_I4://整型
		strValue.Format(_T("%d"), var.intVal);
		break;
	case VT_I8://长整型
		strValue.Format(_T("%d"), var.lVal);
		break;
	case VT_UINT://无符号整型
	case VT_UI4://无符号整型
		strValue.Format(_T("%d"), var.uintVal);
		break;
	case VT_UI8://无符号长整型
		strValue.Format(_T("%d"), var.ulVal);
		break;
	case VT_R4://浮点型
		strValue.Format(_T("%.4f"), var.fltVal);
		break;
	case VT_R8://双精度型
		strValue.Format(_T("%.8f"), var.dblVal);
		break;
	case VT_DECIMAL: //小数
		{
			double val = var.decVal.Lo32;
			val *= (var.decVal.sign == 128)? -1 : 1;
			val /= pow((double)10, (double) var.decVal.scale); 
			strValue.Format(_T("%.4f"), val);
		}
		break;
	case VT_BOOL://布尔型
		strValue = var.lVal ? _T("TRUE") : _T("FALSE");
		break;
	case VT_DATE: //日期型
		dt = var.date;
		da = COleDateTime(dt); 
		strValue = da.Format(_T("%Y-%m-%d %H:%M:%S"));
		break;
	case VT_NULL://NULL值
		strValue = _T("NULL");
		break;
	case VT_EMPTY://空
		strValue = _T("EMPTY");
		break;
	case VT_UNKNOWN://未知类型
	default:
		strValue = _T("UN_KNOW");
		break;
	}
	return strValue;
}


BOOL CAdoRecordSet::SetCacheSize(const long &lCacheSize)
{
	assert(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL && !(GetState() & adStateExecuting))
		{
			return SUCCEEDED(m_pRecordset->put_CacheSize(lCacheSize));
		}
	}
	catch (const _com_error& e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( SetCacheSize Error: %s\n"), e.ErrorMessage());
		return FALSE;
	}
	return FALSE;
}

/*========================================================================
	Name:	确定指定的 Recordset 对象是否支持特定类型的功能。	
    ----------------------------------------------------------
	Params:	CursorOptions   长整型，包括一个或多个下列 CursorOptionEnum 值。
	常量				说明 
	adAddNew			可使用 AddNew 方法添加新记录。 
	adApproxPosition	可读取并设置 AbsolutePosition 和 AbsolutePage 的属性。 
	adBookmark			可使用 Bookmark 属性获得对特定记录的访问。 
	adDelete			可以使用 Delete 方法删除记录。 
	AdHoldRecords		可以检索多个记录或者更改下一个检索位置而不必提交所有挂起的更改。 
	AdMovePrevious		可使用 MoveFirst 和 MovePrevious 方法，以及 Move 或
					GetRows 方法将当前记录位置向后移动而不必使用书签。 
	AdResync			通过 Resync 方法，使用在基本的数据库中可见的数据更新游标。 
	AdUpdate			可使用 Update 方法修改现有的数据。 
	AdUpdateBatch		可以使用批更新(UpdateBatch 和 CancelBatch 方法)将更改组传输给提供者。 
	AdIndex				可以使用 Index 属性命名索引。 
	AdSeek				可以使用 Seek 方法定位 Recordset 中的行。 
    ----------------------------------------------------------
	returns:	返回布尔型值，指示是否支持 CursorOptions 参数所标识的所有功能。
    ----------------------------------------------------------
	Remarks:	使用 Supports 方法确定 Recordset 对象所支持的功能类型。如果
			Recordset 对象支持其相应常量在 CursorOptions 中的功能，那么 Supports
			方法返回 True。否则返回 False。
	注意   尽管 Supports 方法可对给定的功能返回 True，但它不能保证提供者可
	以使功能在所有环境下均有效。Supports 方法只返回提供者是否支持指定的功能
	(假定符合某些条件)。例如，Supports 方法可能指示 Recordset 对象支持更新
	(即使游标基于多个表的合并)，但并且某些列仍然无法更新。
==========================================================================*/
BOOL CAdoRecordSet::Supports(CursorOptionEnum CursorOptions)
{
	assert(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL)
		{
			BOOL bSupports = m_pRecordset->Supports(CursorOptions);
			return bSupports;
		}
	}
	catch (const _com_error& e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Supports Error: %s\n"), e.ErrorMessage());
		return FALSE;
	}
	return FALSE;
}

/*########################################################################
			  ------------------------------------------------
								  数据存取
			  ------------------------------------------------
  ########################################################################*/

BOOL CAdoRecordSet::PutCollect(long index, const _variant_t &value)
{
	assert(m_pRecordset != NULL);
	assert(index < GetFieldsCount());
	try
	{
		if (m_pRecordset != NULL && index < GetFieldsCount()) 
		{
			m_pRecordset->PutCollect(_variant_t(index), value);
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 
	return	FALSE;
}

BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const _variant_t &value)
{
	assert(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL) 
		{
			m_pRecordset->put_Collect(_variant_t(strFieldName), value);
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 
	return	FALSE;
}

BOOL CAdoRecordSet::PutCollect(long index, const LPCTSTR &value)
{
	assert(m_pRecordset != NULL);
	
	return PutCollect(index, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(long index, const BYTE &value)
{
	assert(m_pRecordset != NULL);

	return PutCollect(index, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(long index, const short &value)
{
	assert(m_pRecordset != NULL);
	
	return PutCollect(index, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(long index, const int &value)
{
	assert(m_pRecordset != NULL);
	
	return PutCollect(index, _variant_t(long(value)));
}

BOOL CAdoRecordSet::PutCollect(long index, const long &value)
{
	assert(m_pRecordset != NULL);
	
	return PutCollect(index, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(long index, const float &value)
{
	assert(m_pRecordset != NULL);
	
	return PutCollect(index, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(long index, const double &value)
{
	assert(m_pRecordset != NULL);

	return PutCollect(index, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const LPCTSTR &value)
{
	assert(m_pRecordset != NULL);
	
	return PutCollect(strFieldName, _variant_t(value));
}
BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, COleDateTime &value)
{
	assert(m_pRecordset != NULL);

	return PutCollect(strFieldName, _variant_t(value));
}
BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const BYTE &value)
{
	assert(m_pRecordset != NULL);
	
	return PutCollect(strFieldName, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const short &value)
{
	assert(m_pRecordset != NULL);
	return PutCollect(strFieldName, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const int &value)
{
	assert(m_pRecordset != NULL);

	return PutCollect(strFieldName, _variant_t(long(value)));
}

BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const long &value)
{
	assert(m_pRecordset != NULL);
	
	return PutCollect(strFieldName, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const float &value)
{
	assert(m_pRecordset != NULL);

	return PutCollect(strFieldName, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const double &value)
{
	assert(m_pRecordset != NULL);
	
	return PutCollect(strFieldName, _variant_t(value));
}

/*########################################################################
			  ------------------------------------------------
							       数据存取
			  ------------------------------------------------
  ########################################################################*/

BOOL CAdoRecordSet::GetCollect(long index,  BYTE &value)
{
	assert(m_pRecordset != NULL);

	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(index));
			value = result.bVal;
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::GetCollect(long index,  short &value)
{
	assert(m_pRecordset != NULL);

	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(index));
			value = result.iVal;
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::GetCollect(long index,  int &value)
{
	assert(m_pRecordset != NULL);

	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(index));
			value = result.intVal;
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::GetCollect(long index,  long &value)
{
	assert(m_pRecordset != NULL);

	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(index));
			value = result.lVal;
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::GetCollect(long index,  float &value)
{
	assert(m_pRecordset != NULL);

	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(index));
			value = result.fltVal;
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::GetCollect(long index,  COleDateTime &value)
{
    assert(m_pRecordset != NULL);
 
    try
    {
        if (m_pRecordset != NULL) 
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(index));
            value = (COleDateTime)result.date;
            return TRUE;
        }
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
    } 	
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(long index,  double &value)
{
	assert(m_pRecordset != NULL);

	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(index));
			value = result.dblVal;
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCTSTR strFieldName,  bool &value)
{
	assert(m_pRecordset != NULL);

	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
			value = result.boolVal == _variant_t(true).boolVal;	
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::GetCollect(long index,  bool &value)
{
    assert(m_pRecordset != NULL);

    try
    {
        if (m_pRecordset != NULL) 
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(index));
            value = result.boolVal == _variant_t(true).boolVal;	
            return TRUE;
        }
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
    } 	
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCTSTR strFieldName,  BYTE &value)
{
	assert(m_pRecordset != NULL);

	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
			value = result.bVal;
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCTSTR strFieldName,  short &value)
{
	assert(m_pRecordset != NULL);

	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
			value = result.iVal;
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCTSTR strFieldName,  int &value)
{
	assert(m_pRecordset != NULL);

	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
			value = result.intVal;
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCTSTR strFieldName,  long &value)
{
	assert(m_pRecordset != NULL);

	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
			value = result.lVal;
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCTSTR strFieldName,  float &value)
{
	assert(m_pRecordset != NULL);

	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
			value = result.fltVal;
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCTSTR strFieldName,  double &value)
{
	assert(m_pRecordset != NULL);

	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
			value = result.dblVal;
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCTSTR strFieldName,  unsigned int &value)
{
    assert(m_pRecordset != NULL);

    try
    {
        if (m_pRecordset != NULL) 
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
            value = result.ulVal;
            return TRUE;
        }
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
    } 	
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(long index,  CString &value)
{
	assert(m_pRecordset != NULL);
	
	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(index));
			value = (LPCTSTR)(_bstr_t)result;
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCTSTR strFieldName,  CString &value)
{
    assert(m_pRecordset != NULL);

    try
    {
        if (m_pRecordset != NULL) 
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
            value = (LPCTSTR)(_bstr_t)result;
            return TRUE;
        }
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
    } 	
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCTSTR strFieldName,  COleDateTime &value)
{
	assert(m_pRecordset != NULL);
	assert(GetFieldType(strFieldName) == adDBTimeStamp);

	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
			value = (COleDateTime)result.date;	
			return TRUE;
		}
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        return FALSE;
	} 	
	return FALSE;
}

BOOL CAdoRecordSet::IsOpen()
{
	try
	{
		return (m_pRecordset != NULL && (GetState() & adStateOpen));
	}
	catch (_com_error e)
	{
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( IsOpen方法发生异常: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return FALSE;
}

// 删除记录
BOOL CAdoRecordSet::DeleteCurrentRecord(void)
{
    try
    {
        if(IsOpen())
        {
            if(!Supports(adDelete))
            {
                ADOLOG_ERROR(_T("Cannot delete current record, because provider does not support."));
                return FALSE;
            }

            return Delete(adAffectCurrent);
        }
        ADOLOG_ERROR(_T("Cannot delete current record, because record set is closed."));
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( DeleteCurrentRecord ERROR: %s\n"), e.ErrorMessage());
        return FALSE;
    } 
	
	return FALSE;
}
BOOL CAdoRecordSet::DeleteAllRecord(void)
{
    try
    {
        if(IsOpen())
        {
	        if(!Supports(adDelete))
		        return FALSE;
	        //Delete(adAffectAll);
	        if(GetRecordCount() < 1)
		        return FALSE;
	        MoveFirst();
	        while (!IsEOF()) 
	        {
		        Delete(adAffectCurrent);
		        MoveNext();
	        }
	        return TRUE;
        }
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
    }
	
	return FALSE;
}
// 对数据库进行压缩
BOOL CAdoConnection::CompactDB(LPCTSTR strsource, LPCTSTR strdest)
{
	//try
	//{
	//	IJetEnginePtr jet(__uuidof(JetEngine));
	//	jet->CompactDatabase(strsource, strdest);
	//	//jet->CompactDatabase( "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=d:\\nwind2.mdb;Jet OLEDB:Database Password=test", 
	//	//	"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=d:\\abbc.mdb;Jet OLEDB:Engine Type=4;Jet OLEDB:Database Password=test");
	//}
	//catch(_com_error &e) 
	//{       
 //       ADOLOG_ERROR(GetLastError().GetString());
 //       TRACE(_T(":( CompactDB方法发生异常: %s\n"), e.ErrorMessage());
	//}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// ado command

CAdoCommand::CAdoCommand()
{
    m_pCmd.CreateInstance(_T("ADODB.Command"));
    ASSERT(m_pCmd != NULL);
}

CAdoCommand::CAdoCommand(CAdoConnection* pConnnection)
{
    try
    {
        m_pCmd.CreateInstance(_T("ADODB.Command"));
        ASSERT(m_pCmd);
        m_pCmd->ActiveConnection = pConnnection->GetConnection();
    }
    catch(_com_error &e) 
    {       
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( CAdoCommand ERROR: %s\n"), e.ErrorMessage());
    }
}

CAdoCommand::CAdoCommand(const _ConnectionPtr& pConnnection)
{
    try
    {
        m_pCmd.CreateInstance(_T("ADODB.Command"));
        ASSERT(m_pCmd);
        m_pCmd->ActiveConnection = pConnnection;
    }
    catch(_com_error &e) 
    {       
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( CAdoCommand ERROR: %s\n"), e.ErrorMessage());
    }
}

void CAdoCommand::SetAdoConnection(CAdoConnection* pConnnection)
{
    //ASSERT(pConnnection && pConnnection->IsOpen());
    try
    {
        m_pCmd->ActiveConnection = pConnnection->GetConnection();
    }
    catch(_com_error &e) 
    {       
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( SetAdoConnection ERROR: %s\n"), e.ErrorMessage());
    }
}

void CAdoCommand::SetAdoConnection(const _ConnectionPtr& pConnnection)
{
    ASSERT(pConnnection && pConnnection->GetState() & adStateOpen);
    try
    {
        m_pCmd->ActiveConnection = pConnnection;
    }
    catch(_com_error &e) 
    {       
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( SetAdoConnection ERROR: %s\n"), e.ErrorMessage());
    }
}

void CAdoCommand::SetAdoConnection(LPCTSTR lpszConnStr)
{
    try
    {
        m_pCmd->PutActiveConnection(lpszConnStr);
    }
    catch(_com_error &e) 
    {       
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( SetAdoConnection ERROR: %s\n"), e.ErrorMessage());
    }
}

BOOL CAdoCommand::IsConnectionOpen() /*const*/
{
    try
    {
        return GetActiveConnection() && GetActiveConnection()->GetState() & adStateOpen;
    }
    catch(_com_error &e) 
    {       
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( IsConnectionOpen ERROR: %s\n"), e.ErrorMessage());
    }
    return FALSE;
}

void CAdoCommand::SetCmdTimeout(long nSec)
{
    try
    {
        m_pCmd->PutCommandTimeout(nSec);
    }
    catch(_com_error &e) 
    {       
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( SetCmdTimeout ERROR: %s\n"), e.ErrorMessage());
    }
}

CString CAdoCommand::GetLastError()
{
    CAdoConnPtr connPtr = GetAdoConnectionPtr();
    if (connPtr)
    {
        return connPtr->GetLastError();
    }
    
    return _T("");
}

_ParameterPtr CAdoCommand::CreateIntParam(int nVal, 
                                      LPCTSTR lpszParamName, 
                                      ParameterDirectionEnum direction/* = adParamInput */)
{
    ASSERT (m_pCmd);
    _ParameterPtr p;

    try
    {
        p = m_pCmd->CreateParameter(lpszParamName, adInteger, direction, sizeof(int), nVal);
        return p;
    }
    catch(_com_error &e) 
    {       
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( CreateIntParam ERROR: %s\n"), e.ErrorMessage());
    }
    return p;
}

_ParameterPtr CAdoCommand::CreateVarStrParam(const CString& strVal, 
                                              unsigned int nMaxCharCnt,
                                         LPCTSTR lpszParamName, 
                                         ParameterDirectionEnum direction/* = adParamInput */
                                        )
{
    ASSERT(m_pCmd);
    _ParameterPtr p;
    try
    {
        _variant_t v(strVal.GetString());
        _ParameterPtr p = m_pCmd->CreateParameter(lpszParamName, adVarChar, direction, nMaxCharCnt, v);
        return p;
    }
    catch(_com_error &e) 
    {       
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( CreateVarStrParam ERROR: %s\n"), e.ErrorMessage());
    }
    return p;
}

_ParameterPtr CAdoCommand::CreateParam(_variant_t v, DataTypeEnum t, 
                                       unsigned int nMaxCharCnt, 
                                       LPCTSTR lpszParamName /* = NULL */, 
                                       ParameterDirectionEnum direction/* = adParamInput */)
{
    ASSERT(m_pCmd);
    _ParameterPtr p;
    try
    {
        _ParameterPtr p = m_pCmd->CreateParameter(lpszParamName, t, direction, nMaxCharCnt, v);
        return p;
    }
    catch(_com_error &e) 
    {       
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( CreateVarStrParam ERROR: %s\n"), e.ErrorMessage());
    }
    return p;
}

BOOL CAdoCommand::CreateAppendParam(_variant_t v, 
                                    DataTypeEnum t, 
                                    unsigned int nMaxCharCnt,
                                    LPCTSTR lpszParamName /* = NULL */,
                                    ParameterDirectionEnum direction/* = adParamInput */)
{
    ASSERT(m_pCmd);
    _ParameterPtr p = CreateParam(v, t, nMaxCharCnt, lpszParamName, direction);
    if (p)
    {
        return AppendParam(p);
    }

    return FALSE;
}

BOOL CAdoCommand::CreateAppendParam(COleDateTime time, 
                                    LPCTSTR lpszParamName /* = NULL */,
                                    ParameterDirectionEnum direction/* = adParamInput */)
{
    return CreateAppendParam(_variant_t(time), adDBTimeStamp, sizeof(COleDateTime), lpszParamName, direction);
}

BOOL CAdoCommand::CreateAppendParam(int n, 
                                    LPCTSTR lpszParamName /* = NULL */, 
                                    ParameterDirectionEnum direction/* = adParamInput */)
{
    return CreateAppendParam(_variant_t(n), adInteger, sizeof(int), lpszParamName, direction);
}

_variant_t CAdoCommand::GetParam(LPCTSTR lpszParamName)
{
    ASSERT(m_pCmd);
    try
    {
        return SUCCEEDED(m_pCmd->Parameters->GetItem(_variant_t(lpszParamName)));
    }
    catch(_com_error &e) 
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetParam ERROR: %s\n"), e.ErrorMessage());
    }

    _variant_t v;
    v.vt = VT_NULL;
    return v;
}

BOOL CAdoCommand::AppendParam(_ParameterPtr p)
{
    ASSERT(m_pCmd);
    try
    {
        return SUCCEEDED(m_pCmd->GetParameters()->Append(p));
    }
    catch(_com_error &e) 
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( CreateVarStrParam ERROR: %s\n"), e.ErrorMessage());
        return FALSE;
    }

    return TRUE;
}

CAdoConnPtr CAdoCommand::GetAdoConnectionPtr()
{
    CAdoConnPtr ptr;
    try
    {
        _ConnectionPtr connptr = GetActiveConnection();
        if (connptr)
        {
            ptr.reset(new CAdoConnection(connptr));
        }
    }
    catch(_com_error &e) 
    {       
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetActiveConnection ERROR: %s\n"), e.ErrorMessage());
    }
    return ptr;
}

_ConnectionPtr CAdoCommand::GetActiveConnection()
{
    _ConnectionPtr ptr;
    try
    {
        ptr = m_pCmd->GetActiveConnection();
    }
    catch(_com_error &e) 
    {       
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( GetActiveConnection ERROR: %s\n"), e.ErrorMessage());
    }

    return ptr;
}

CAdoCmdSQLText::CAdoCmdSQLText()
{
}

CAdoCmdSQLText::CAdoCmdSQLText(const _ConnectionPtr& pConnnection)
: CAdoCommand(pConnnection)
{
}

CAdoCmdSQLText::CAdoCmdSQLText(CAdoConnection* pConnnection)
: CAdoCommand(pConnnection)
{
}

CAdoRecSetPtr CAdoCmdSQLText::Execute(LPCTSTR lpszCmdText)
{
    CAdoRecSetPtr ret;
    ASSERT(m_pCmd);
    try
    {
        m_pCmd->PutCommandText(lpszCmdText);
        _RecordsetPtr ptr = m_pCmd->Execute(NULL, NULL, adCmdText);
        ret.reset(new CAdoRecordSet(ptr));
        return ret;
    }
    catch (_com_error e)
    {
		ADOLOG_ERROR(lpszCmdText);
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Execute ERROR: %s\n"), e.ErrorMessage());
    }

    return ret;
}

BOOL CAdoCmdSQLText::Execute(LPCTSTR lpszCmdText, CAdoRecSetPtr& rsPtrOut)
{
    ASSERT(m_pCmd);
    try
    {
        m_pCmd->PutCommandText(lpszCmdText);
        _RecordsetPtr ptr = m_pCmd->Execute(NULL, NULL, adCmdText);
        rsPtrOut.reset(new CAdoRecordSet(ptr));
        return TRUE;
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Execute ERROR: %s\n"), e.ErrorMessage());
    }

    return FALSE;
}

CAdoCmdTable::CAdoCmdTable(CAdoConnection* pConnnection)
: CAdoCommand(pConnnection)
{

}

CAdoCmdTable::CAdoCmdTable(const _ConnectionPtr& pConnnection)
: CAdoCommand(pConnnection)
{
}

CAdoCmdTable::CAdoCmdTable()
{
}

CAdoRecSetPtr CAdoCmdTable::Execute(LPCTSTR lpszTableName)
{
    CAdoRecSetPtr ret;
    ASSERT(m_pCmd);
    try
    {
        m_pCmd->PutCommandText(lpszTableName);
        _RecordsetPtr ptr = m_pCmd->Execute(NULL, NULL, adCmdTable);
        ret.reset(new CAdoRecordSet(ptr));
        return ret;
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Execute ERROR: %s\n"), e.ErrorMessage());
    }

    return ret;
}

BOOL CAdoCmdTable::Execute(LPCTSTR lpszTableName,  CAdoRecSetPtr& ret)
{
    ASSERT(m_pCmd);
    try
    {
        m_pCmd->PutCommandText(lpszTableName);
        _RecordsetPtr ptr = m_pCmd->Execute(NULL, NULL, adCmdTable);
        ret.reset(new CAdoRecordSet(ptr));
        return TRUE;
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Execute ERROR: %s\n"), e.ErrorMessage());
    }

    return FALSE;
}

CAdoCmdStoredProc::CAdoCmdStoredProc()
{
}

CAdoCmdStoredProc::CAdoCmdStoredProc(const _ConnectionPtr& pConnnection)
: CAdoCommand(pConnnection)
{
}

CAdoCmdStoredProc::CAdoCmdStoredProc(CAdoConnection* pConnnection)
: CAdoCommand(pConnnection)
{

}

void CAdoCmdStoredProc::SetStoredProcName(LPCTSTR lpszStoredProcName)
{
    try
    {
        m_pCmd->PutCommandText(lpszStoredProcName);
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( PutCommandText ERROR: %s\n"), e.ErrorMessage());
    }
}

BOOL CAdoCmdStoredProc::RefreshParams()
{
    ASSERT(m_pCmd);
    try
    {
        HRESULT hr = m_pCmd->Parameters->Refresh();
        return SUCCEEDED(hr);
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Execute ERROR: %s\n"), e.ErrorMessage());
    }
    return FALSE;
}

void CAdoCmdStoredProc::SetParam(LPCTSTR lpszParamName, int val)
{
    ASSERT(m_pCmd);
    try
    {
        m_pCmd->GetParameters()->Item[_variant_t(lpszParamName)]->PutValue(val);
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( SetParam ERROR: %s\n"), e.ErrorMessage());
    }
}

void CAdoCmdStoredProc::SetParam(LPCTSTR lpszParamName, LPCTSTR val)
{
    ASSERT(m_pCmd);
    try
    {
        m_pCmd->GetParameters()->Item[_variant_t(lpszParamName)]->PutValue(val);
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( SetParam ERROR: %s\n"), e.ErrorMessage());
    }
}

CAdoRecSetPtr CAdoCmdStoredProc::Execute()
{
    CAdoRecSetPtr ret;
    ASSERT(m_pCmd);
    try
    {
        _RecordsetPtr ptr = m_pCmd->Execute(NULL, NULL, adCmdStoredProc);
        ret.reset(new CAdoRecordSet(ptr));
        return ret;
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Execute ERROR: %s\n"), e.ErrorMessage());
    }

    return ret;
}

BOOL CAdoCmdStoredProc::Execute(CAdoRecSetPtr& ret)
{
    ASSERT(m_pCmd);
    try
    {
        _RecordsetPtr ptr = m_pCmd->Execute(NULL, NULL, adCmdStoredProc);
        ret.reset(new CAdoRecordSet(ptr));
        return TRUE;
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Execute ERROR: %s\n"), e.ErrorMessage());
    }

    return FALSE;
}

BOOL CAdoCmdStoredProc::Execute2()
{
    ASSERT(m_pCmd);
    try
    {
        HRESULT hr = m_pCmd->Execute(NULL, NULL, adCmdStoredProc);
        if (SUCCEEDED(hr))
        {
            return TRUE;
        }
        else
        {
            ADOLOG_ERROR(GetLastError().GetString());
        }
    }
    catch (_com_error e)
    {
        ADOLOG_ERROR(GetLastError().GetString());
        TRACE(_T(":( Execute ERROR: %s\n"), e.ErrorMessage());
    }

    return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// ado parameter

CAdoParam::CAdoParam()
{
    m_paramPtr.CreateInstance(_T("ADODB.Parameter"));
}

CAdoParam::~CAdoParam()
{
}

CAdoParam::CAdoParam(_ParameterPtr p)
{
    m_paramPtr = p;
}