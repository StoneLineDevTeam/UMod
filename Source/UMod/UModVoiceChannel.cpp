#include "UMod.h"
#include "UModVoiceChannel.h"

bool UUModVoiceChannel::CleanUp(const bool bForDestroy)
{
	Super::CleanUp(bForDestroy);
	return true;
}

void UUModVoiceChannel::ReceivedBunch(FInBunch& Bunch)
{
	Super::ReceivedBunch(Bunch);
}

void UUModVoiceChannel::Tick()
{
	Super::Tick();
}

void UUModVoiceChannel::SendVoicePacket(FOutVoicePacket Pack)
{
	FOutBunch Bunch = SerializeVoicePacket(Pack);
	Bunch.bReliable = true;
	SendBunch(&Bunch, true);
}

FOutBunch UUModVoiceChannel::SerializeVoicePacket(FOutVoicePacket Pack)
{
	FOutBunch Bunch = FOutBunch(this, 0);
	Bunch.WriteIntWrapped(Pack.BufferSize, 65535); //Hard code uint16 maximum value as I will not support writing more than 65535 bytes to the network as that would be WAYS TOO MUCH !
	//We will instead write less bytes per packets directly read from micro.
	for (uint16 i = 0; i < Pack.BufferSize; i++) {
		Bunch.WriteBit(Pack.VoiceData[i]);
	}
	return Bunch;
}

FInVoicePacket UUModVoiceChannel::ReadVoicePacket(FInBunch &Bunch)
{
	uint16 Size = (uint16)Bunch.ReadInt(65535);
	FInVoicePacket Pack;
	Pack.BufferSize = Size;
	Pack.VoiceDataToPlay = new uint8[Size];
	for (uint16 i = 0; i < Size; i++) {
		uint8 byte = Bunch.ReadBit();
		Pack.VoiceDataToPlay[i] = byte;
	}
	return Pack;
}
