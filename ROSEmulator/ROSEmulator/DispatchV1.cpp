#include "stdafx.h"
#include "DispatchV1.h"
#include "Dispatcher.h"
#include "RosNetwork.h"
BOOL CDispatchV1::Initialize(CDispatcher* pDispatcher, CString sLocalIp, UINT nLocalPort)
{
	m_pDispatcher = pDispatcher;

	return Create(nLocalPort, SOCK_DGRAM, FD_READ, sLocalIp);
}


void CDispatchV1::Terminate()
{
	Close();
}


int CDispatchV1::Send_Message(CString sMessage, CString sIp)
{
	return SendTo((LPCSTR)sMessage, sMessage.GetLength(), HANDLER_LISTEN_PORT, sIp);
}


void CDispatchV1::OnReceive(int nErrorCode)
{
	if (nErrorCode == 0 && m_pDispatcher)
		m_pDispatcher->OnUdpClientReceive();
	CAsyncSocket::OnReceive(nErrorCode);
}


