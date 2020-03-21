// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Sockets.h"
#include "Networking/Public/Networking.h"
#include "Enum.h"
#include "GameInstance_Main.generated.h"

/**
 * 
 */
UCLASS()
class TIC_TAC_TOE_API UGameInstance_Main : public UGameInstance
{
	GENERATED_BODY()
	
protected:
	virtual void OnStart() override;
	virtual void Shutdown() override;
	//bool StartTCPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort);
	//FSocket* CreateTCPListener(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, const int32 ReceiveBufferSize = 2 * 1024 * 1024);

	//Timer functions, could be threads
	//FTimerHandle TimerHandle_Listener;
	//FTimerHandle TimerHandle_SocketListener;
	//void TCPListener(); 	            //can thread this eventually
	//void TCPSocketListener();		//can thread this eventually


	////Format String IP4 to number array
	//bool FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]);
	//FSocket* ListenerSocket;
	//FSocket* ConnectionSocket;

	int32 nUID;
	FString Username;
	FString EMail;

	FTimerHandle TimerHandle_Tick;
	void TimerFun_Tick();

	bool CreateSocket(const FString IP, const int32 nPort);
	bool Send(FString message);

	FIPv4Address ServerIP;
	FSocket* pSocketClient;
	TQueue<FString> Queue_Message;

public:
	void ShowMessage(const FString Message, const EMessageType MessageType);
	void AddMessageToQueue(const FString message);

	int32 GetUID() const { return nUID; }

	void Login(const FString username, const FString password);
	void Register(const FString username, const FString password, const FString email);
	void CreateRoom(const FString RoomTitle);
	void JoinRoom(const int32 roomID);
	void SendMessage(const int32 roomID, const FString message);
	void Ready(const int32 roomID);
	void Start(const int32 roomID);
	void Play(const int32 roomID, const FString team, const int32 x, const int32 y);
};
