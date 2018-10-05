// server.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <Winsock2.h>	//Socket的函数调用　
#include <windows.h>

#define BUF_SIZE 6400  //  缓冲区大小 

#pragma comment (lib, "ws2_32")		// 使用WINSOCK2.H时，则需要库文件WS2_32.LIB

DWORD WINAPI Rcv( LPVOID lpParam )
{
	SOCKET sClient = *(SOCKET*)lpParam;
	int retVal;
	char bufRecv[BUF_SIZE]; 
	memset( bufRecv, 0, sizeof( bufRecv ) );
	while(1)
	{
		retVal = recv( sClient, bufRecv, BUF_SIZE, 0 );
		if ( retVal == SOCKET_ERROR ) {
			printf( "recive faild!\n" );
			break;
		} else {
			printf( "收到客户端消息：%s\n", bufRecv );
		} 
	}
	return 0;
}

DWORD WINAPI Snd( LPVOID lpParam )
{
	SOCKET sClient = *(SOCKET*)lpParam;
	int retVal;
	char bufSend[BUF_SIZE]; 
	memset( bufSend, 0, sizeof( bufSend ) );
	while(1)
	{
		gets( bufSend );
		retVal = send( sClient, bufSend, strlen(bufSend)+sizeof(char), 0 );
		if ( retVal == SOCKET_ERROR ) {
			printf( "send faild!\n" );
			break;
		} 
	}
	return 0;
}


int main(int argc, char* argv[])
{
	// 初始化套接字动态库
	WSADATA wsaData;
	if ( WSAStartup(MAKEWORD(2, 2), &wsaData) != 0 ) {
		printf( "winsock load faild!\n" );
		return 1;
	}
	
	//  创建服务段套接字 
	SOCKET sServer = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( sServer == INVALID_SOCKET ) {
		printf( "socket faild!\n" );
		WSACleanup();
		return -1;
	}
	
	//  服务端地址 
	sockaddr_in addrServ;

	addrServ.sin_family = AF_INET; 
	addrServ.sin_port = htons( 9999 );
	addrServ.sin_addr.s_addr = htonl( INADDR_ANY ); 
	
	//  绑定套接字 
	if ( bind( sServer, ( const struct sockaddr* )&addrServ, sizeof(addrServ) ) == SOCKET_ERROR ) {
		printf( "bind faild!\n" );
		closesocket( sServer );
		WSACleanup(); 
		return -1;
	} 
	
	printf("Server is On IP:[%s],port:[%d]\n",inet_ntoa(addrServ.sin_addr),ntohs(addrServ.sin_port)); 
	
	//  监听套接字  数字表示最多能监听客户个数 
	if ( listen( sServer, 5 ) == SOCKET_ERROR ) {
		printf( "listen faild!\n" );
		closesocket( sServer );
		WSACleanup();
		return -1; 
	} 
	
	SOCKET sClient; //  客户端套接字
	
	sockaddr_in addrClient;
	int addrClientLen = sizeof( addrClient );
	
	
	sClient = accept( sServer, ( sockaddr FAR* )&addrClient, &addrClientLen ); 
	if ( sClient == INVALID_SOCKET ) {
		printf( "accept faild!\n" );
		closesocket( sServer );
		WSACleanup();
		return -1; 
	}
	printf("accepted client IP:[%s],port:[%d]\n",inet_ntoa(addrClient.sin_addr),ntohs(addrClient.sin_port));
	
	HANDLE hThread1, hThread2;
	DWORD dwThreadId1, dwThreadId2;

	hThread1 = ::CreateThread(NULL, NULL, Snd, (LPVOID*)&sClient, 0, &dwThreadId1);
	hThread2 = ::CreateThread(NULL, NULL, Rcv, (LPVOID*)&sClient, 0, &dwThreadId2);

	::WaitForSingleObject(hThread1, INFINITE);
	::WaitForSingleObject(hThread2, INFINITE);
	::CloseHandle(hThread1);
	::CloseHandle(hThread2);
		
	closesocket( sClient ); 
	WSACleanup(); // 资源释放

	return 0;
}

