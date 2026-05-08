#pragma once
#ifndef __PROTOCOL_H
#define __PROTOCOL_H

enum class EPacketType : unsigned short
{
	Plus,
	Minus,
	Multiply,
	Divide,
	Result,
	Max
};

#pragma pack(push, 1)
struct PacketHeader
{
	unsigned short Size;
	unsigned short Code;
};

struct TwoNumber
{
	int FirstNumber;
	int SecondNumber;
};
#pragma pack(pop)

#endif // __PROTOCOL_H