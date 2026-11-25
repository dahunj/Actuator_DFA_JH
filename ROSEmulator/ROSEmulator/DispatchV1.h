#pragma once
class CDispatcher;
class CDispatchV1 : public CAsyncSocket
{
public:

	CDispatchV1() : m_pDispatcher(NULL)
	{
	}

	BOOL Initialize(CDispatcher* pDispatcher, CString sLocalIp, UINT nLocalPort);

	void Terminate();
	int Send_Message(CString sMessage, CString sIp);

protected:

	virtual void OnReceive(int nErrorCode);
private:

	CDispatcher* m_pDispatcher;
};
