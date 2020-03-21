#pragma once

#include "CoreMinimal.h"
#include "Enum.generated.h"

UENUM(BlueprintType)
enum class EMessageType : uint8 {
	E_SUCCESS UMETA(DisplayName = "Success"),
	E_ERROR UMETA(DisplayName = "Error")
};
