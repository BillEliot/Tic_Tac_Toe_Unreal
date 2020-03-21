// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance_Main.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Threads/ReceiveThread.h"
#include "HUD_Main.h"
#include "UI/UserWidget_HUD.h"
#include "UI/UserWidget_Hall.h"
#include "UI/UserWidget_Room.h"
#include "GameMode/GameModeBase_Game.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void UGameInstance_Main::OnStart() {
	Super::OnStart();

	if (CreateSocket("127.0.0.1", 1081)) {
		UE_LOG(LogTemp, Warning, TEXT("Socket Succeed"));
	}
	else UE_LOG(LogTemp, Warning, TEXT("Socket Failed"));
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Tick, this, &UGameInstance_Main::TimerFun_Tick, 1.f, true);
}

void UGameInstance_Main::Shutdown() {
	Super::Shutdown();

	FReceiveThread::Shutdown();
}
//Format IP String as Number Parts
//bool UGameInstance_Main::FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]) {
//	//IP Formatting
//	TheIP.Replace(TEXT(" "), TEXT(""));
//	TArray<FString> Parts;
//	TheIP.ParseIntoArray(Parts, TEXT("."), true);
//	if (Parts.Num() != 4) return false;
//
//	//String to Number Parts
//	for (int32 i = 0; i < 4; ++i) {
//		Out[i] = FCString::Atoi(*Parts[i]);
//	}
//
//	return true;
//}


bool UGameInstance_Main::CreateSocket(const FString IP, const int32 nPort) {
	FIPv4Address::Parse(IP, ServerIP);
	TSharedRef<FInternetAddr> ServerAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	ServerAddress->SetIp(ServerIP.Value);
	ServerAddress->SetPort(nPort);

	pSocketClient = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
	if (pSocketClient->Connect(*ServerAddress)) {
		UE_LOG(LogTemp, Warning, TEXT("Connect Successfully"));
		FReceiveThread::Start(pSocketClient, this);
		return true;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Connect Failed"));
		return false;
	}
}

bool UGameInstance_Main::Send(FString message) {
	TCHAR* messageChar = message.GetCharArray().GetData();
	int32 nSize = FCString::Strlen(messageChar);
	int32 nSent = 0;

	if (pSocketClient->Send((uint8*)TCHAR_TO_UTF8(messageChar), nSize, nSent)) return true;
	else return false;
}

void UGameInstance_Main::TimerFun_Tick() {
	// Handle Message
	if (!Queue_Message.IsEmpty()) {
		FString Message;
		Queue_Message.Dequeue(Message);

		TSharedPtr<FJsonObject> Json;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);

		FString sign;
		int32 nCode = 0;
		TSharedPtr<FJsonObject> data;
		if (FJsonSerializer::Deserialize(Reader, Json)) {
			sign = Json->GetStringField("sign");
			nCode = Json->GetIntegerField("code");
			data = Json->GetObjectField("data");
		}

		APlayerController* pPlayerController = GetWorld()->GetFirstPlayerController();
		if (sign == "register") {
			switch (nCode) {
			case 0:
				ShowMessage(UTF8_TO_TCHAR("注册成功"), EMessageType::E_SUCCESS);
				break;
			case 1:
				ShowMessage(UTF8_TO_TCHAR("注册失败：用户名已存在"), EMessageType::E_ERROR);
				break;
			case 2:
				ShowMessage(UTF8_TO_TCHAR("注册失败：邮箱已存在"), EMessageType::E_ERROR);
				break;
			}
		}
		else if (sign == "login") {
			switch (nCode) {
			case 0:
			{
				if (pPlayerController) {
					AHUD_Main* pHUD = Cast<AHUD_Main>(pPlayerController->GetHUD());
					if (pHUD) {
						nUID = data->GetIntegerField("uid");
						Username = data->GetStringField("username");
						EMail = data->GetStringField("email");

						pHUD->ToHall(data->GetStringField("username"), data->GetStringField("email"), data->GetObjectField("rooms"));
					}
				}
			}
			break;
			case 1:
				ShowMessage(UTF8_TO_TCHAR("登陆失败：密码错误"), EMessageType::E_ERROR);
				break;
			case 2:
				ShowMessage(UTF8_TO_TCHAR("登陆失败：用户不存在"), EMessageType::E_ERROR);
				break;
			}
		}
		else if (sign == "create_room") {
			switch (nCode) {
			case 0:
				if (pPlayerController) {
					// Add new room
					UUserWidget_Hall* pHall = Cast<AHUD_Main>(pPlayerController->GetHUD())->GetHallWidget();
					pHall->AddNewRoom(data);
					// Request to enter the new room if is the master
					if (data->GetObjectField("data")->GetIntegerField("master_uid") == nUID) {
						FString Msg = "join_room:" + FString::FromInt(data->GetIntegerField("room_id")) + "," + FString::FromInt(nUID) + "," + Username;
						Send(Msg);
					}
				}

				break;
			}
		}
		else if (sign == "join_room") {
			switch (nCode) {
			case 0:
				if (pPlayerController) {
					UUserWidget_Room* pRoom = Cast<AHUD_Main>(pPlayerController->GetHUD())->GetRoomWidget();
					pRoom->SetInfo(data);
				}
				
				break;
			}
		}
		else if (sign == "update_room_player") {
			switch (nCode) {
			case 0:
				if (pPlayerController) {
					UUserWidget_Room* pRoom = Cast<AHUD_Main>(pPlayerController->GetHUD())->GetRoomWidget();
					pRoom->AddNewPlayer(data);
				}
				break;
			}
		}
		else if (sign == "update_room_player_count") {
			switch (nCode) {
			case 0:
				if (pPlayerController) {
					UUserWidget_Hall* pHall = Cast<AHUD_Main>(pPlayerController->GetHUD())->GetHallWidget();
					pHall->UpdateRoomPlayerCount(data->GetIntegerField("room_id"));
				}
				break;
			}
		}
		else if (sign == "room_message") {
			switch (nCode) {
			case 0:
				if (pPlayerController) {
					UUserWidget_Room* pRoom = Cast<AHUD_Main>(pPlayerController->GetHUD())->GetRoomWidget();
					UUserWidget_HUD* pHUD = Cast<AHUD_Main>(pPlayerController->GetHUD())->GetHUDWidget();
					pRoom->ReceiveMessage(data);
					pHUD->ReceiveMessage(data);
				}
				break;
			}
		}
		else if (sign == "room_ready") {
			switch (nCode) {
			case 0:
				if (pPlayerController) {
					UUserWidget_Room* pRoom = Cast<AHUD_Main>(pPlayerController->GetHUD())->GetRoomWidget();
					pRoom->UpdateReady(data);
				}
				break;
			}
		}
		else if (sign == "room_start") {
			switch (nCode) {
			case 0:
				if (pPlayerController) {
					if (data->GetIntegerField("master_uid") == nUID) Cast<AGameModeBase_Game>(GetWorld()->GetAuthGameMode())->GameStarted(true);
					else Cast<AGameModeBase_Game>(GetWorld()->GetAuthGameMode())->GameStarted(false);
				}
				break;
			case 1:
				ShowMessage(UTF8_TO_TCHAR("您不是房主"), EMessageType::E_ERROR);
				break;
			case 2:
				ShowMessage(UTF8_TO_TCHAR("不是所有人都已准备"), EMessageType::E_ERROR);
				break;
			}
		}
		else if (sign == "game_play") {
			switch (nCode) {
			case 0:
				Cast<AGameModeBase_Game>(GetWorld()->GetAuthGameMode())->Play(data);
				Cast<AGameModeBase_Game>(GetWorld()->GetAuthGameMode())->GameDone(data);
				break;
			case 1:
				Cast<AGameModeBase_Game>(GetWorld()->GetAuthGameMode())->Play(data);
				break;
			case 2:
				ShowMessage(UTF8_TO_TCHAR("不是你的回合"), EMessageType::E_ERROR);
				break;
			}
		}
	}
}

//bool UGameInstance_Main::Receive(FString& message) {
//	if (!pSocketClient) return false;
//
//	TArray<uint8> ReceiveData;
//	uint32 size;
//	uint8 element = 0;
//	while (pSocketClient->HasPendingData(size)) {
//		ReceiveData.Init(element, FMath::Min(size, 65507u));
//
//		int32 nRead = 0;
//		pSocketClient->Recv(ReceiveData.GetData(), ReceiveData.Num(), nRead);
//	}
//
//	if (ReceiveData.Num() <= 0) return false;
//
//	FString log = "Total Data read! num: " + FString::FromInt(ReceiveData.Num() <= 0);
//	UE_LOG(LogTemp, Warning, TEXT("Recv log:   %s"), *log);
//
//	const FString ReceivedUE4String = StringFromBinaryArray(ReceiveData);
//
//	log = "Server:" + ReceivedUE4String;
//	UE_LOG(LogTemp, Warning, TEXT("*** %s"), *log);
//
//	message = ReceivedUE4String;
//	return true;
//}

// Start TCP Receiver
//bool UGameInstance_Main::StartTCPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort) {
//	ListenerSocket = CreateTCPListener(YourChosenSocketName, TheIP, ThePort);
//	if (!ListenerSocket) {
//		UE_LOG(LogTemp, Warning, TEXT("StartTCPReceiver>> Listen socket could not be created! ~> %s %d"), *TheIP, ThePort);
//		return false;
//	}
//
//	//Start the Listener! //thread this eventually
//	//GetWorld()->GetTimerManager().SetTimer(TimerHandle_Listener, this, &UGameInstance_Main::TCPListener, 0.01, true);
//	FAcceptThread::Start(ListenerSocket, ConnectionSocket);
//	return true;
//}
//
//FSocket* UGameInstance_Main::CreateTCPListener(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, const int32 ReceiveBufferSize) {
//	uint8 IP4Nums[4];
//	if (!FormatIP4ToNumber(TheIP, IP4Nums)) {
//		UE_LOG(LogTemp, Warning, TEXT("Invalid IP! Expecting 4 parts separated by."));
//		return false;
//	}
//
//	// Create Socket
//	FIPv4Endpoint Endpoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), ThePort);
//	FSocket* ListenSocket = FTcpSocketBuilder(*YourChosenSocketName).AsReusable().BoundToEndpoint(Endpoint).Listening(8);
//
//	// Set Buffer Size
//	int32 NewSize = 0;
//	ListenSocket->SetReceiveBufferSize(ReceiveBufferSize, NewSize);
//
//	return ListenSocket;
//}
//
//void UGameInstance_Main::TCPListener() {
//
//}
//
////Rama's TCP Socket Listener
//void UGameInstance_Main::TCPSocketListener() {
//	if (!ConnectionSocket) return;
//
//	TArray<uint8> ReceivedData;
//	uint32 Size;
//	while (ConnectionSocket->HasPendingData(Size)) {
//		ReceivedData.Init(FMath::Min(Size, 65507u), Size);
//
//		int32 Read = 0;
//		ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
//	}
//
//	if (ReceivedData.Num() <= 0) return;
//
//	UE_LOG(LogTemp, Warning, TEXT("Total Data read: %d"), ReceivedData.Num());
//
//	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	//						Rama's String From Binary Array
//	const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
//	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//	UE_LOG(LogTemp, Warning, TEXT("As String: %s"), *ReceivedUE4String);
//}

void UGameInstance_Main::ShowMessage(const FString Message, const EMessageType MessageType) {
	APlayerController* pPlayerController = GetWorld()->GetFirstPlayerController();
	if (pPlayerController) Cast<AHUD_Main>(pPlayerController->GetHUD())->ShowMessage(Message, MessageType);
}

void UGameInstance_Main::AddMessageToQueue(const FString message) {
	Queue_Message.Enqueue(message);
}

void UGameInstance_Main::Login(const FString username, const FString password) {
	FString Message = "login:" + username + "," + password;
	Send(Message);
}

void UGameInstance_Main::Register(const FString username, const FString password, const FString email) {
	FString Message = "register:" + username + "," + password + "," + email;
	Send(Message);
}

void UGameInstance_Main::CreateRoom(const FString RoomTitle) {
	FString Message = "create_room:" + RoomTitle + "," + FString::FromInt(nUID) + "," + Username;
	Send(Message);
}

void UGameInstance_Main::JoinRoom(const int32 roomID) {
	FString Message = "join_room:" + FString::FromInt(roomID) + "," + FString::FromInt(nUID) + "," + Username;
	Send(Message);
}

void UGameInstance_Main::SendMessage(const int32 roomID, const FString message) {
	FString Message = "room_message:" + FString::FromInt(roomID) + "," + Username  + "," + message;
	Send(Message);
}

void UGameInstance_Main::Ready(const int32 roomID) {
	FString Message = "room_ready:" + FString::FromInt(roomID) + "," + FString::FromInt(nUID);
	Send(Message);
}

void UGameInstance_Main::Start(const int32 roomID) {
	FString Message = "room_start:" + FString::FromInt(roomID) + "," + FString::FromInt(nUID);
	Send(Message);
}

void UGameInstance_Main::Play(const int32 roomID, const FString team, const int32 x, const int32 y) {
	FString Message = "game_play:" + FString::FromInt(nUID) + "," + FString::FromInt(roomID) + "," + team + "," + FString::FromInt(x) + "," + FString::FromInt(y);
	Send(Message);
}
