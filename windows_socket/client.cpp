// client.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <Winsock2.h>	//Socket的函数调用　
#include <windows.h>

#define BUF_SIZE 6400

#pragma comment (lib, "ws2_32")		// 使用WINSOCK2.H时，则需要库文件WS2_32.LIB

DWORD WINAPI Rcv( LPVOID lpParam )
{
	SOCKET sHost = *(SOCKET*)lpParam;
	int retVal;
	char bufRecv[BUF_SIZE]; 
	memset( bufRecv, 0, sizeof( bufRecv ) );
	while(1)
	{
		retVal = recv( sHost, bufRecv, BUF_SIZE, 0 );
		if ( retVal == SOCKET_ERROR ) {
			printf( "recive faild!\n" );
			break;
		} else {
			printf( "收到服务器消息：%s\n", bufRecv );
		} 
	}
	return 0;
}

DWORD WINAPI Snd( LPVOID lpParam )
{
	SOCKET sHost = *(SOCKET*)lpParam;
	int retVal;
	char bufSend[BUF_SIZE]; 
	memset( bufSend, 0, sizeof( bufSend ) );
	while(1)
	{
		gets( bufSend );
		retVal = send( sHost, bufSend, strlen(bufSend)+sizeof(char), 0 );
		if ( retVal == SOCKET_ERROR ) {
			printf( "send faild!\n" );
			break;
		} 
	}
	return 0;
}

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	if ( WSAStartup( MAKEWORD(2,2), &wsaData ) != 0 ) {
		printf( "Winsock load faild!\n" );
		return 1;
	}
	
	//  服务器套接字 
	SOCKET sHost = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( sHost == INVALID_SOCKET ) {
		printf( "socket faild!\n" );
		WSACleanup();
		return -1;
	}
	
	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	//  注意   当把客户端程序发到别人的电脑时 此处IP需改为服务器所在电脑的IP 
	servAddr.sin_addr.S_un.S_addr = inet_addr( "127.0.0.1" );
	servAddr.sin_port = htons( 9999 );
	
	//  连接服务器
	if ( connect( sHost, (LPSOCKADDR)&servAddr, sizeof( servAddr ) ) == SOCKET_ERROR ) {
		printf( "connect faild!\n" );
		closesocket(sHost);
		WSACleanup();
		return -1;
	} 
	printf("连接到服务器 IP:[%s],port:[%d]\n",inet_ntoa(servAddr.sin_addr),ntohs(servAddr.sin_port));

	HANDLE hThread1, hThread2;
	DWORD dwThreadId1, dwThreadId2;

	hThread1 = ::CreateThread( NULL, NULL, Snd, (LPVOID)&sHost, 0, &dwThreadId1 );
	hThread2 = ::CreateThread( NULL, NULL, Rcv, (LPVOID)&sHost, 0, &dwThreadId2 );
	
	::WaitForSingleObject( hThread1, INFINITE );
	::WaitForSingleObject( hThread2, INFINITE );
	::CloseHandle(hThread1);
	::CloseHandle(hThread2);


	closesocket(sHost);
	WSACleanup(); 
	return 0;
}

