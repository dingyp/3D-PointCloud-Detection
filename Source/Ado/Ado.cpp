// Ado.cpp : ���� DLL �ĳ�ʼ�����̡�
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
	///���� Connection ����---------------------------
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
	Name:		���ӵ�����Դ.
	-----------------------------------------------------
	Params:		strConnect: �����ַ���,����������Ϣ.
				lOptions:	��ѡ�������÷����������ӽ���֮��(�첽)��������
						����֮ǰ(ͬ��)���ء�����������ĳ��������
		����					˵�� 
		adConnectUnspecified	(Ĭ��)ͬ����ʽ�����ӡ� 
		adAsyncConnect			�첽��ʽ�����ӡ�Ado�� ConnectComplete ��
							����֪ͨ��ʱ������ӡ� 
=====================================================================*/
BOOL CAdoConnection::Connect(LPCTSTR strConnect, long lOptions)
{
	assert(m_pConnection != NULL);
	try
	{
		// �������ݿ�---------------------------------------------
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
        // �������ݿ�---------------------------------------------
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
    Name: ִ�������洢����

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
	Name:	�ر�������Դ������.
	-----------------------------------------------------
	Remarks: ʹ�� Close �����ɹر� Connection �����Ա��ͷ����й�����ϵͳ��
		Դ���رն��󲢷ǽ������ڴ���ɾ�������Ը��������������ò����ڴ˺���
		�δ򿪡�Ҫ��������ڴ�����ȫɾ���������Release������
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
	Name:	�ر����Ӳ��ͷŶ���.
	-----------------------------------------------------
	Remarks: �ر����Ӳ��ͷ�connection�������������ϴ���������ȫ���
		��connection����
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
	Remarks:	�뿼�� CAdoRecordSet �� Cancel ����.
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
	Name:	ȡ��������Ĵ�����Ϣ.
	-----------------------------------------------------
	Remarks: �κ��漰 ADO ����Ĳ��������Բ���һ�������ṩ�ߴ��󡣲�����
		��ʱ,���Խ�һ������ Error �������� Connection ����� Errors ����
		�С����� ADO ������������ʱ������� Errors ���ϣ����ҽ��µ� Error 
		�������� Errors �����С� 
			ÿ�� Error ��������ض����ṩ�ߴ��󣬶����� ADO ����ADO ����
		������������ʱ���쳣��������С�
			û�в�������� ADO ������ Errors ����û��Ӱ�졣ʹ�� Clear ��ʽ
		���ֹ���� Errors ���ϡ�
			Errors �����е� Error ���󼯺�������Ӧ�������ʱ���������д���
		�о� Errors ������ָ�������ʹ���������̸���ȷ��ȷ�����������ԭ
		�򼰴�����Դ����ִ���ʵ���ԭ���衣
			ĳЩ���Ժͷ�����������Ϊ Errors �����е� Error ������ʾ�ľ��棬
		��������ֹ�����ִ�С��ڵ��� Recordset �����ϵ� Resync��UpdateBatch
		�� CancelBatch ������Connection �� Open ������������ Recordset ����
		�ϵ� Filter ����ǰ������� Errors �����ϵ� Clear �������������Ϳ���
		�Ķ� Errors ���ϵ� Count �����Բ��Է��صľ��档
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

//��鵱ǰ�����Ƿ���Ч���糬ʱ��������Ͽ����������쳣�ȣ��ᵼ������Ͽ�
BOOL CAdoConnection::VerifyConnection()
{
	static CTime tLast = CTime::GetCurrentTime();

	CTime t1 = CTime::GetCurrentTime();
	CTimeSpan t= t1- tLast;

	//Ϊ�˱����μ�飬���Ч��, ��������ʱ����
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
					//�Ƿ��и����ʻ��߿�����С�����
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
    Name:		������Դ��ȡ���ݿ���Ϣ.
	-----------------------------------------------------
	Params:		QueryType  ��Ҫ���е�ģʽ��ѯ����,����Ϊ�������ⳣ��.
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
	adSchemaProviderSpecific	 �μ�˵�� 
	adSchemaProviderTypes		DATA_TYPE
								BEST_MATCH 
	adSchemaReferentialConstraints CONSTRAINT_CATALOG
								CONSTRAINT_SCHEMA
								CONSTRAINT_NAME 
	adSchemaSchemata			CATALOG_NAME
								SCHEMA_NAME
								SCHEMA_OWNER 
	adSchemaSQLLanguages		<��> 
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
	returns:	���ذ������ݿ���Ϣ�� Recordset ����. Recordset ����ֻ������̬
			�α��.
	-----------------------------------------------------
	Remarks:	OpenSchema��������������Դ�йص���Ϣ��������ڷ������ϵı�
			�Լ����е��е���Ϣ, �������ݽ����ο�, �Ӿ��������Դ���ܻ��в�ͬ��
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
									������
			  ------------------------------------------------
	BeginTrans		- ��ʼ������
	CommitTrans		- �����κθ��Ĳ�������ǰ������Ҳ��������������
	RollbackTrans	- ȡ����ǰ�������������κθ��Ĳ�����������Ҳ��������
					������

		һ�������� BeginTrans �������ڵ��� CommitTrans �� RollbackTrans ��
	������֮ǰ�����ݿ⽫���������ύ�������κθ��ġ�
		����֧��Ƕ����������ݿ���˵�����Ѵ򿪵������е��� BeginTrans ����
	����ʼ�µ�Ƕ�����񡣷���ֵ��ָʾǶ�ײ�Σ�����ֵΪ 1 ��ʾ�Ѵ򿪶�������
	(�����񲻱���һ��������Ƕ��)������ֵΪ 2 ��ʾ�Ѵ򿪵ڶ�������(Ƕ��
	�ڶ��������е�����)���������ơ����� CommitTrans �� RollbackTrans ֻӰ
	�����´򿪵������ڴ����κθ��߲�����֮ǰ����رջ�ؾ�ǰ����
		���� CommitTrans ���������������ϴ򿪵������������ĸ��Ĳ���������
	���� RollbackTrans������ԭ�������������ĸ��Ĳ�����������δ������
	ʱ���������κ�һ�ַ���������������
		ȡ���� Connection ����� Attributes ���ԣ����� CommitTrans �� 
	RollbackTrans �����������Զ�������������� Attributes ��������Ϊ 
	adXactCommitRetaining�����ݿ��� CommitTrans ���ú���Զ�����������
	��� Attributes ��������Ϊ adXactAbortRetaining�����ݿ��ڵ��� 
	RollbackTrans ֮���Զ�����������
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
					 CAdoRecordSet class ���캯��
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
		- strSQL:		���� Command ����ı�����,SQL ���,����,�洢���̵�
					�û�־� Recordset �ļ���.
		- CursorType:   ��ѡ.CursorTypeEnum ֵ, ȷ����RecordsetʱӦ
					��ʹ�õ��α�����.��Ϊ���г���֮һ.
		����				˵�� 
		-----------------------------------------------
		adOpenForwardOnly	�򿪽���ǰ�����α�. 
		adOpenKeyset		�򿪼��������α�. 
		adOpenDynamic		�򿪶�̬�����α�. 
		adOpenStatic		�򿪾�̬�����α�. 
		-----------------------------------------------
		- LockType:     ��ѡ,ȷ����RecordsetʱӦ��ʹ�õ���������(����)��
					LockTypeEnumֵ,��Ϊ���г���֮һ.
		����				˵�� 
		-----------------------------------------------
		adLockReadOnly		ֻ�� �� ���ܸı�����. 
		adLockPessimistic	����ʽ���� - ͨ��ͨ���ڱ༭ʱ������������Դ�ļ�¼. 
		adLockOptimistic	����ʽ���� - ֻ�ڵ��� Update ����ʱ��������¼. 
		adLockBatchOptimistic ����ʽ������ - ����������ģʽ(����������ģʽ
						���). 
		-----------------------------------------------
		- lOption		��ѡ. ������ֵ, ����ָʾ strSQL ���������Ϳ�Ϊ����
					����֮һ.
		����				˵�� 
		-------------------------------------------------
		adCmdText			ָʾstrSQLΪ�����ı�, ����ͨ��SQL���. 
		adCmdTable			ָʾADO����SQL��ѯ�Ա����strSQL�������ı��з�
						��������. 
		adCmdTableDirect	ָʾ�����ĸ�����strSQL�������ı��з���������. 
		adCmdStoredProc		ָʾstrSQLΪ�洢����. 
		adCmdUnknown		ָʾstrSQL�����е���������Ϊδ֪. 
		adCmdFile			ָʾӦ����strSQL���������ļ��лָ�����(�����)
						Recordset. 
		adAsyncExecute		ָʾӦ�첽ִ��strSQL. 
		adAsyncFetch		ָʾ����ȡ Initial Fetch Size ������ָ���ĳ�ʼ
						������,Ӧ���첽��ȡ����ʣ�����.������������δ��
						ȡ,��Ҫ���߳̽�������ֱ�������¿���. 
		adAsyncFetchNonBlocking ָʾ��Ҫ�߳�����ȡ�ڼ��δ����. ���������
						������δ��ȡ,��ǰ���Զ��Ƶ��ļ�ĩβ. 
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
	Name:		�رմ򿪵Ķ����κ���ض���.
	-----------------------------------------------------
	Remarks:	ʹ��Close�����ɹر�Recordset�����Ա��ͷ����й�����ϵͳ��Դ.
		�رն��󲢷ǽ������ڴ���ɾ��, ���Ը��������������ò����ڴ˺��ٴδ�
		��.Ҫ��������ڴ�����ȫɾ��, �ɽ������������Ϊ Nothing.
		    ʹ��Close�����ر�Recordset�����ͬʱ, ���ͷŹ��������ݺͿ�����
		��ͨ�����ض�Recordset��������ݽ��еĶ�������.���ɵ���Open������
		�´򿪾�����ͬ���Ի����޸����Ե�Recordset.��Recordset����رպ�,��
		���κ���Ҫ��α�ķ�������������.
			���������������ģʽ�½��б༭,����Close��������������, Ӧ����
		����Update��CancelUpdat ����.������������ڼ�ر�Recordset����, ��
		���ϴ�UpdateBatch���������������޸Ľ�ȫ����ʧ.
			���ʹ��Clone���������Ѵ򿪵�Recordset����ĸ���,�ر�ԭʼRecordset
		���丱������Ӱ���κ���������.
			Ҫ��������ڴ�����ȫɾ���������Release������
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
	Name:	�ر����Ӳ��ͷŶ���.
	-----------------------------------------------------
	Remarks: �ر����Ӳ��ͷ�CAdoRecordSet�������������ϴ���������ȫ���
		��CAdoRecordSet����
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
							   ��¼�����²���
			  ------------------------------------------------
  ########################################################################*/

/*===================================================================
	Remarks:	����µļ�¼.��AddNew()����, Ȼ����PutCollect()����ÿ
				�е�ֵ, �����Update()�������ݿ�.
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
	Remarks:	�ڵ���AddNew()�ȷ����󣬵��ô˷�����ɸ��»��޸�.
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
	Name:		ȡ���ڵ��� Update ����ǰ�Ե�ǰ��¼���¼�¼�������κθ���.
	-----------------------------------------------------
	Remarks:	ʹ�� CancelUpdate ������ȡ���Ե�ǰ��¼�������κθ��Ļ�
	��������ӵļ�¼.�ڵ��� Update �������޷������Ե�ǰ��¼���¼�¼
	�����ĸ���,���Ǹ����ǿ����� RollbackTrans �����ؾ�������һ����,
	�����ǿ����� CancelBatch ����ȡ���������µ�һ����.
	����ڵ��� CancelUpdate ����ʱ����¼�¼,����� AddNew ֮ǰ�ĵ�ǰ
	��¼���ٴγ�Ϊ��ǰ��¼.
	�����δ���ĵ�ǰ��¼������¼�¼,���� CancelUpdate ��������������.
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
	Name:		ȡ�������������.
	-----------------------------------------------------
	Params:		AffectRecords   ��ѡ�� AffectEnum ֵ,����CancelBatch 
		������Ӱ���¼����Ŀ,��Ϊ���г���֮һ��
		����			˵�� 
		-------------------------------------------------
		AdAffectCurrent ��ȡ����ǰ��¼�Ĺ������. 
		AdAffectGroup	�����㵱ǰ Filter �������õļ�¼ȡ���������.
						ʹ�ø�ѡ��ʱ,���뽫 Filter ��������Ϊ�Ϸ���Ԥ
						���峣��֮һ. 
		AdAffectAll		Ĭ��ֵ.ȡ�� Recordset ���������м�¼�Ĺ����
						��,�����ɵ�ǰ Filter �������������ص��κμ�¼. 
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
	Params:		AffectRecords:  AffectEnumֵ, ȷ��Delete������Ӱ��ļ�¼��
		Ŀ,��ֵ���������г���֮һ.
		����				˵�� 
		-------------------------------------------------
		AdAffectCurrent		Ĭ��.��ɾ����ǰ��¼. 
		AdAffectGroup		ɾ�����㵱ǰFilter�������õļ�¼.Ҫʹ�ø�ѡ��,
						���뽫 Filter ��������Ϊ��Ч��Ԥ���峣��֮һ. 
		adAffectAll			ɾ�����м�¼. 
		adAffectAllChapters ɾ�������Ӽ���¼. 
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
								��¼����������
			  ------------------------------------------------
    Remarks:	
		ʹ��MoveFirst��������ǰ��¼λ���ƶ���Recordse�еĵ�һ����¼.
		ʹ��MoveLast��������ǰ��¼λ���ƶ���Recordset�е����һ����¼.
	Recordset�������֧����ǩ��������ƶ�;������ø÷�������������.

		ʹ��MoveNext��������ǰ��¼��ǰ�ƶ�һ����¼(��Recordset�ĵײ�).���
	���һ����¼�ǵ�ǰ��¼���ҵ���MoveNext����,��ADO����ǰ��¼���õ�
	Recordset(EOFΪTrue)��β��¼֮��.��EOF�����Ѿ�ΪTrueʱ��ͼ��ǰ�ƶ�
	����������.
		ʹ��MovePrevious��������ǰ��¼λ������ƶ�һ����¼(���¼���Ķ���).
	Recordset �������֧����ǩ������α��ƶ�;���򷽷����ý���������.
	����׼�¼�ǵ�ǰ��¼���ҵ���MovePrevious����,��ADO����ǰ��¼������
	Recordset(BOFΪTrue)���׼�¼֮ǰ.��BOF����ΪTrueʱ����ƶ���������
	��. ���Recordset����֧����ǩ������α��ƶ�,��MovePrevious������
	��������.
		�����¼���ǽ���ǰ��,�����û�ϣ��֧����ǰ��������,�����ʹ��
	CacheSize ���Դ�����¼����,ͨ�� Move����֧������α��ƶ�.����
	�����¼�Ǽ��ص��ڴ��е�,����Ӧ���ⲻ��Ҫ�ػ���̫���¼.���Ե���
	����ǰ Recordset ����� MoveFirst ����;��������ʹ�ṩ������ִ����
	�� Recordset ���������.
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
	Name:		�ƶ� Recordset �����е�ǰ��¼��λ�á�
    ----------------------------------------------------------
	Params:		
		- lRecords    �����ų����ͱ��ʽ��ָ����ǰ��¼λ���ƶ��ļ�¼����
		- Start    ��ѡ���ַ���������ͣ����ڼ�����ǩ��Ҳ��Ϊ���� BookmarkEnum ֵ֮һ��
		����				˵�� 
		adBookmarkCurrent	Ĭ�ϡ��ӵ�ǰ��¼��ʼ�� 
		adBookmarkFirst		���׼�¼��ʼ�� 
		AdBookmarkLast		��β��¼��ʼ�� 
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
								  ��¼������
			  ------------------------------------------------
  ########################################################################*/

/*========================================================================
	Name:		�����п�Ӧ�ö���,˵�������״̬�Ǵ򿪻��ǹر�.��ִ��
	�첽������ Recordset ����,˵����ǰ�Ķ���״̬�����ӡ�ִ�л��ǻ�ȡ.
	-----------------------------------------------------
	returns:	�������г���֮һ�ĳ�����ֵ.
		����				˵�� 
		----------------------------------
		adStateClosed		ָʾ�����ǹرյ�. 
		adStateOpen			ָʾ�����Ǵ򿪵�. 
		adStateConnecting	ָʾ Recordset ������������. 
		adStateExecuting	ָʾ Recordset ��������ִ������. 
		adStateFetching		ָʾ Recordset ����������ڱ���ȡ. 
	-----------------------------------------------------
	Remarks:		������ʱʹ�� State ����ȷ��ָ������ĵ�ǰ״̬.��
	������ֻ����.Recordset ����� State ���Կ��������ֵ.����,���
	����ִ�����,�����Խ��� adStateOpen �� adStateExecuting �����ֵ.
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
	Name:		ָʾ�й������»��������������ĵ�ǰ��¼��״̬.
	-----------------------------------------------------
	returns:	��������һ������ RecordStatusEnum ֵ֮��.
		����						˵�� 
		-------------------------------------------------
		adRecOK						�ɹ��ظ��¼�¼. 
		adRecNew					��¼���½���. 
		adRecModified				��¼���޸�. 
		adRecDeleted				��¼��ɾ��. 
		adRecUnmodified				��¼û���޸�. 
		adRecInvalid				������ǩ��Ч,��¼û�б���. 
		adRecMultipleChanges		����Ӱ������¼,��˼�¼δ������. 
		adRecPendingChanges			���ڼ�¼���ù���Ĳ���,���δ������. 
		adRecCanceled				���ڲ�����ȡ��,δ�����¼. 
		adRecCantRelease			�������м�¼����,û�б����¼�¼. 
		adRecConcurrencyViolation	���ڿ���ʽ������ʹ����,��¼δ������. 
		adRecIntegrityViolation		�����û�Υ��������Լ��,��¼δ������. 
		adRecMaxChangesExceeded		���ڴ��ڹ���������,��¼δ������. 
		adRecObjectOpen				������򿪵Ĵ�������ͻ,��¼δ������. 
		adRecOutOfMemory			���ڼ�����ڴ治��,��¼δ������. 
		adRecPermissionDenied		�����û�û���㹻��Ȩ��,��¼δ������. 
		adRecSchemaViolation		���ڼ�¼Υ���������ݿ�Ľṹ,���δ������. 
		adRecDBDeleted				��¼�Ѿ�������Դ��ɾ��. 
	-----------------------------------------------------
	Remarks:	ʹ�� Status ���Բ鿴���������б��޸ĵļ�¼����Щ���ı�
	����.Ҳ��ʹ�� Status ���Բ鿴��������ʱʧ�ܼ�¼��״̬.����,���� 
	Recordset ����� Resync��UpdateBatch �� CancelBatch ����,�������� 
	Recordset ����� Filter ����Ϊ��ǩ����.ʹ�ø�����,�ɼ��ָ����¼
	Ϊ��ʧ�ܲ���������.
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
	Name:		��ȡ��ǰ��¼���м�¼��Ŀ
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
	Name:		��ȡ��ǰ��¼�����ֶ���Ŀ
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
	Name:		ָ���Ƿ����ڼ�¼��ͷ
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
	Name:		ָ���Ƿ����ڼ�¼��β
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
	Remarks:	Recordset ������� Field ������ɵ� Fields ����.ÿ�� 
	Field �����Ӧ Recordset ���е�һ��.
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
	Remarks:	ȡ��ָ�����ֶε��ֶ���.
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
	returns:	���� Field ����Attributes ����Ϊֻ������ֵ����Ϊ��������һ������ FieldAttributeEnum ֵ�ĺ͡�
	  ����				˵�� 
	  adFldMayDefer			ָʾ�ֶα��ӳ٣�������ӵ��������¼������Դ����
						�ֶ�ֵ��������ʽ������Щ�ֶ�ʱ�Ž��м����� 
	  adFldUpdatable		ָʾ����д����ֶΡ� 
	  adFldUnknownUpdatable ָʾ�ṩ���޷�ȷ���Ƿ����д����ֶΡ� 
	  adFldFixed			ָʾ���ֶΰ����������ݡ� 
	  adFldIsNullable		ָʾ���ֶν��� Null ֵ�� 
	  adFldMayBeNull		ָʾ���ԴӸ��ֶζ�ȡ Null ֵ�� 
	  adFldLong				ָʾ���ֶ�Ϊ���������ֶΡ���ָʾ����ʹ�� AppendChunk �� GetChunk ������ 
	  adFldRowID			ָʾ�ֶΰ����־õ��б�ʶ�����ñ�ʶ���޷���д��
						���ҳ��˶��н��б�ʶ(���¼�š�Ψһ��ʶ����)�ⲻ
						�����������ֵ�� 
	  adFldRowVersion		ָʾ���ֶΰ����������ٸ��µ�ĳ��ʱ������ڱ�ǡ� 
	  adFldCacheDeferred	ָʾ�ṩ�߻������ֶ�ֵ������������Ի���Ķ�ȡ�� 
    ----------------------------------------------------------
	Remarks:	ȡ�� Field ����һ���������ԡ�
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
	Name:		ָʾ Field ����������ĳ��ȡ�
    ----------------------------------------------------------
	returns:	����ĳ���ֶζ���ĳ���(���ֽ���)�ĳ�����ֵ��
    ----------------------------------------------------------
	Remarks:	ʹ�� DefinedSize ���Կ�ȷ�� Field ���������������
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
	Name:	ȡ���ֶε�ֵ��ʵ�ʳ��ȡ�
    ----------------------------------------------------------
	returns:	���س�����ֵ��ĳЩ�ṩ���������ø������Ա�Ϊ BLOB ����Ԥ��
			�ռ䣬�ڴ������Ĭ��ֵΪ 0��
    ----------------------------------------------------------
	Remarks:	ʹ�� ActualSize ���Կɷ��� Field ����ֵ��ʵ�ʳ���.��������
			�ֶ�,ActualSize ����Ϊֻ������� ADO �޷�ȷ�� Field ����ֵ��ʵ
			�ʳ��ȣ�ActualSize ���Խ����� adUnknown��
				�����·�����ʾ��ActualSize ��  DefinedSize ����������ͬ��
			adVarChar ������������󳤶�Ϊ 50 ���ַ��� Field ���󽫷���Ϊ 
			50 �� DefinedSize ����ֵ�����Ƿ��ص� ActualSize ����ֵ�ǵ�ǰ��
			¼���ֶ��д洢�����ݵĳ��ȡ�
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
	returns:	��������ֵ֮һ����Ӧ�� OLE DB ���ͱ�ʶ�����±��˵�����������и�����
	  ����				˵�� 
	  ---------------------------------------------------------
	  adArray			����������һ������߼� OR ��ָʾ���������������͵�
					��ȫ���� (DBTYPE_ARRAY)�� 
	  adBigInt			8 �ֽڴ����ŵ����� (DBTYPE_I8)�� 
	  adBinary			������ֵ (DBTYPE_BYTES)�� 
	  adBoolean			������ֵ (DBTYPE_BOOL)�� 
	  adByRef			����������һ������߼� OR ��ָʾ������������������
					�ݵ�ָ�� (DBTYPE_BYREF)�� 
	  adBSTR			�Կս�β���ַ��� (Unicode) (DBTYPE_BSTR)�� 
	  adChar			�ַ���ֵ (DBTYPE_STR)�� 
	  adCurrency		����ֵ (DBTYPE_CY)���������ֵ�С����λ�ù̶���С��
					���Ҳ�����λ���֡���ֵ����Ϊ 8 �ֽڷ�ΧΪ10,000 �Ĵ���
					������ֵ�� 
	  adDate			����ֵ (DBTYPE_DATE)�����ڰ�˫��������ֵ�����棬��
					��ȫ����ʾ�� 1899 �� 12 �� 30 ��ʼ����������С��������
					һ�쵱�е�Ƭ��ʱ�䡣 
	  adDBDate			����ֵ (yyyymmdd) (DBTYPE_DBDATE)�� 
	  adDBTime			ʱ��ֵ (hhmmss) (DBTYPE_DBTIME)�� 
	  adDBTimeStamp		ʱ��� (yyyymmddhhmmss �� 10 �ڷ�֮һ��С��)(DBTYPE_DBTIMESTAMP). 
	  adDecimal			���й̶����Ⱥͷ�Χ�ľ�ȷ����ֵ (DBTYPE_DECIMAL)�� 
	  adDouble			˫���ȸ���ֵ (DBTYPE_R8)�� 
	  adEmpty			δָ��ֵ (DBTYPE_EMPTY)�� 
	  adError			32 - λ������� (DBTYPE_ERROR)�� 
	  adGUID			ȫ��Ψһ�ı�ʶ�� (GUID) (DBTYPE_GUID)�� 
	  adIDispatch		OLE ������ Idispatch �ӿڵ�ָ�� (DBTYPE_IDISPATCH)�� 
	  adInteger			4 �ֽڵĴ��������� (DBTYPE_I4)�� 
	  adIUnknown		OLE ������ IUnknown �ӿڵ�ָ�� (DBTYPE_IUNKNOWN)��
	  adLongVarBinary	��������ֵ�� 
	  adLongVarChar		���ַ���ֵ�� 
	  adLongVarWChar	�Կս�β�ĳ��ַ���ֵ�� 
	  adNumeric			���й̶����Ⱥͷ�Χ�ľ�ȷ����ֵ (DBTYPE_NUMERIC)�� 
	  adSingle			�����ȸ���ֵ (DBTYPE_R4)�� 
	  adSmallInt		2 �ֽڴ��������� (DBTYPE_I2)�� 
	  adTinyInt			1 �ֽڴ��������� (DBTYPE_I1)�� 
	  adUnsignedBigInt	8 �ֽڲ����������� (DBTYPE_UI8)�� 
	  adUnsignedInt		4 �ֽڲ����������� (DBTYPE_UI4)�� 
	  adUnsignedSmallInt 2 �ֽڲ����������� (DBTYPE_UI2)�� 
	  adUnsignedTinyInt 1 �ֽڲ����������� (DBTYPE_UI1)�� 
	  adUserDefined		�û�����ı��� (DBTYPE_UDT)�� 
	  adVarBinary		������ֵ�� 
	  adVarChar			�ַ���ֵ�� 
	  adVariant			�Զ������� (DBTYPE_VARIANT)�� 
	  adVector			����������һ������߼� OR �У�ָʾ������ DBVECTOR 
					�ṹ(�� OLE DB ����)���ýṹ����Ԫ�صļ������������� 
					(DBTYPE_VECTOR) ���ݵ�ָ�롣 
	  adVarWChar		�Կս�β�� Unicode �ַ����� 
	  adWChar			�Կս�β�� Unicode �ַ��� (DBTYPE_WSTR)�� 
    ----------------------------------------------------------
	Remarks:	����ָ���е��������͡�
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
	Name:	ȡ��ָ���е��ֶζ����ָ��.	
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
	Name:		ȡ��ִ�й�����첽 Execute �� Open �����ĵ���.
	-----------------------------------------------------
	Remarks:	ʹ�� Cancel ������ִֹ���첽 Execute �� Open ��������(��ͨ
		�� adAsyncConnect��adAsyncExecute �� adAsyncFetch �������õķ���).
		�������ͼ��ֹ�ķ�����û��ʹ�� adAsyncExecute,�� Cancel ����������
		ʱ����.
			�±���ʾ���ض����Ͷ�����ʹ�� Cancel ����ʱ����ֹ������.
		�������Ϊ		����ֹ�Ը÷�������һ���첽���� 
		-------------------------------------------------
		Command			Execute 
		Connection		Execute �� Open 
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
								   ��������
			  ------------------------------------------------
  ########################################################################*/

/*========================================================================
	Remarks:	ȡ�õ�ǰ��¼��ָ���ֶε�ֵ��ת�����ַ���
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
	Remarks:	��_variant_t�ͱ�����ʽ����CString���ַ���.
==========================================================================*/
CString CAdoRecordSet::Format(_variant_t var)
{
	DATE dt;
	COleDateTime da;
	CString strValue;
	
	switch (var.vt)
	{
	case VT_BSTR://�ַ���
	case VT_LPSTR://�ַ���
	case VT_LPWSTR://�ַ���
		strValue = (LPCTSTR)(_bstr_t)var;
		break;
	case VT_I1://������                            //yk 20140918 fix mysql bool filed read error
	case VT_I2://������
		strValue.Format(_T("%d"), var.iVal);
		break;
	case VT_UI2://�޷��Ŷ�����
		strValue.Format(_T("%d"), var.uiVal);
		break;
	case VT_VOID://
	case VT_INT://����
	case VT_I4://����
		strValue.Format(_T("%d"), var.intVal);
		break;
	case VT_I8://������
		strValue.Format(_T("%d"), var.lVal);
		break;
	case VT_UINT://�޷�������
	case VT_UI4://�޷�������
		strValue.Format(_T("%d"), var.uintVal);
		break;
	case VT_UI8://�޷��ų�����
		strValue.Format(_T("%d"), var.ulVal);
		break;
	case VT_R4://������
		strValue.Format(_T("%.4f"), var.fltVal);
		break;
	case VT_R8://˫������
		strValue.Format(_T("%.8f"), var.dblVal);
		break;
	case VT_DECIMAL: //С��
		{
			double val = var.decVal.Lo32;
			val *= (var.decVal.sign == 128)? -1 : 1;
			val /= pow((double)10, (double) var.decVal.scale); 
			strValue.Format(_T("%.4f"), val);
		}
		break;
	case VT_BOOL://������
		strValue = var.lVal ? _T("TRUE") : _T("FALSE");
		break;
	case VT_DATE: //������
		dt = var.date;
		da = COleDateTime(dt); 
		strValue = da.Format(_T("%Y-%m-%d %H:%M:%S"));
		break;
	case VT_NULL://NULLֵ
		strValue = _T("NULL");
		break;
	case VT_EMPTY://��
		strValue = _T("EMPTY");
		break;
	case VT_UNKNOWN://δ֪����
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
	Name:	ȷ��ָ���� Recordset �����Ƿ�֧���ض����͵Ĺ��ܡ�	
    ----------------------------------------------------------
	Params:	CursorOptions   �����ͣ�����һ���������� CursorOptionEnum ֵ��
	����				˵�� 
	adAddNew			��ʹ�� AddNew ��������¼�¼�� 
	adApproxPosition	�ɶ�ȡ������ AbsolutePosition �� AbsolutePage �����ԡ� 
	adBookmark			��ʹ�� Bookmark ���Ի�ö��ض���¼�ķ��ʡ� 
	adDelete			����ʹ�� Delete ����ɾ����¼�� 
	AdHoldRecords		���Լ��������¼���߸�����һ������λ�ö������ύ���й���ĸ��ġ� 
	AdMovePrevious		��ʹ�� MoveFirst �� MovePrevious �������Լ� Move ��
					GetRows ��������ǰ��¼λ������ƶ�������ʹ����ǩ�� 
	AdResync			ͨ�� Resync ������ʹ���ڻ��������ݿ��пɼ������ݸ����αꡣ 
	AdUpdate			��ʹ�� Update �����޸����е����ݡ� 
	AdUpdateBatch		����ʹ��������(UpdateBatch �� CancelBatch ����)�������鴫����ṩ�ߡ� 
	AdIndex				����ʹ�� Index �������������� 
	AdSeek				����ʹ�� Seek ������λ Recordset �е��С� 
    ----------------------------------------------------------
	returns:	���ز�����ֵ��ָʾ�Ƿ�֧�� CursorOptions ��������ʶ�����й��ܡ�
    ----------------------------------------------------------
	Remarks:	ʹ�� Supports ����ȷ�� Recordset ������֧�ֵĹ������͡����
			Recordset ����֧������Ӧ������ CursorOptions �еĹ��ܣ���ô Supports
			�������� True�����򷵻� False��
	ע��   ���� Supports �����ɶԸ����Ĺ��ܷ��� True���������ܱ�֤�ṩ�߿�
	��ʹ���������л����¾���Ч��Supports ����ֻ�����ṩ���Ƿ�֧��ָ���Ĺ���
	(�ٶ�����ĳЩ����)�����磬Supports ��������ָʾ Recordset ����֧�ָ���
	(��ʹ�α���ڶ����ĺϲ�)��������ĳЩ����Ȼ�޷����¡�
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
								  ���ݴ�ȡ
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
							       ���ݴ�ȡ
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
        TRACE(_T(":( IsOpen���������쳣: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return FALSE;
}

// ɾ����¼
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
// �����ݿ����ѹ��
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
 //       TRACE(_T(":( CompactDB���������쳣: %s\n"), e.ErrorMessage());
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