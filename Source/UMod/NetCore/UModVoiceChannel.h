#pragma once
#include "UMod.h"
#include "UModVoiceChannel.generated.h"

struct FOutVoicePacket {
	uint8 *VoiceData; //The voice data directly captured from MicroPhone (as bytes)
	uint16 BufferSize; //How many bytes we have read from the micro phone
};

struct FInVoicePacket {
	uint8 *VoiceDataToPlay; //Voice data ready to be inputed to a USoundWaveProcedural
	uint16 BufferSize; //The size of the buffer sent (USoundWaveProcedural needs it)
};

UCLASS()
class UUModVoiceChannel : public UChannel {

	GENERATED_BODY()
	
	UUModVoiceChannel(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : UChannel(ObjectInitializer)
	{
		// Register with the network channel system
		ChannelClasses[5] = GetClass();
		ChType = CHANNEL_VOICE;
	}
protected:
	virtual bool CleanUp(const bool bForDestroy) override;

	virtual void ReceivedBunch(FInBunch& Bunch) override;

	virtual void Tick() override;

	virtual FString Describe() override
	{
		return FString(TEXT("UModVoIP: ")) + Super::Describe();
	}
public:
	void SendVoicePacket(FOutVoicePacket Pack);
private:
	FOutBunch SerializeVoicePacket(FOutVoicePacket Pack);
	FInVoicePacket ReadVoicePacket(FInBunch &Bunch);
};