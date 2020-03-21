#include "ReceiveThread.h"
#include "Engine/World.h"
#include "TimerManager.h"

FReceiveThread* FReceiveThread::pRunnable = nullptr;
FReceiveThread::FReceiveThread(FSocket* socketClient, UGameInstance_Main* gameInstance) : StopTaskCounter(0) {
	pSocketClient = socketClient;
	pGameInstance = gameInstance;
	pThread = FRunnableThread::Create(this, TEXT("FReceiveThread"), 0, TPri_BelowNormal);
}

bool FReceiveThread::Init() {
	UE_LOG(LogTemp, Warning, TEXT("Thread Init"));
	return true;
}

FReceiveThread* FReceiveThread::Start(FSocket* socketClient, UGameInstance_Main* gameInstance) {
	if (!pRunnable && FPlatformProcess::SupportsMultithreading()) {
		pRunnable = new FReceiveThread(socketClient, gameInstance);
	}
	return pRunnable;
}

uint32 FReceiveThread::Run() {
	TArray<uint8> ReceiveData;
	uint32 size = 0;
	while (StopTaskCounter.GetValue() == 0) {
		if (pSocketClient && pSocketClient->HasPendingData(size)) {
			ReceiveData.Init(0, FMath::Min(size, 65507u));

			int32 nRead = 0;
			pSocketClient->Recv(ReceiveData.GetData(), ReceiveData.Num(), nRead);
			if (ReceiveData.Num() > 0) {
				ReceiveData.Add(0);
				ReceiveMessage = StringFromBinaryArray(ReceiveData);
				UE_LOG(LogTemp, Warning, TEXT("%s"), *ReceiveMessage);
				pGameInstance->AddMessageToQueue(ReceiveMessage);
			}
			ReceiveData.Empty();
			size = 0;
		}
	}

	return 0;
}

void FReceiveThread::Shutdown() {
	if (pRunnable) {
		pRunnable->Stop();
		pRunnable->pThread->WaitForCompletion();

		delete pRunnable;
		pRunnable = nullptr;
	}
}

void FReceiveThread::Stop() {
	StopTaskCounter.Increment();
}

FString FReceiveThread::StringFromBinaryArray(TArray<uint8> BinaryArray) {
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
}
