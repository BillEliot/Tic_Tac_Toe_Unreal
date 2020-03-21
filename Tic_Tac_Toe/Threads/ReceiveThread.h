#pragma once

#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "GameInstance_Main.h"


class TIC_TAC_TOE_API FReceiveThread : public FRunnable
{
public:
    FReceiveThread(FSocket* socketClient, UGameInstance_Main* gameInstance);
    ~FReceiveThread() {
        delete pThread;
        pThread = nullptr;
    }

protected:
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;

    static FReceiveThread* pRunnable;
    FRunnableThread* pThread;
    FThreadSafeCounter StopTaskCounter;

    FString StringFromBinaryArray(TArray<uint8> BinaryArray);

    FSocket* pSocketClient;
    UGameInstance_Main* pGameInstance;
    UPROPERTY()
        APlayerController* pPlayerController;
    FString ReceiveMessage;
public:
    static FReceiveThread* Start(FSocket* socketClient, UGameInstance_Main* gameInstance);
    static void Shutdown();
};
