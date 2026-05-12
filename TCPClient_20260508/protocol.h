#pragma once
#ifndef __PROTOCOL_H
#define __PROTOCOL_H

enum class EOperationType : unsigned short
{
	Plus,
	Minus,
	Multiply,
	Divide,
	Result,
	Max
};

enum class EPacketType : unsigned short
{
	CS_Calculate = 1,
	SC_Calculate
};

#pragma pack(push, 1)
struct PacketHeader
{
	unsigned short Size;
	unsigned short PacketType;
};

struct CS_Calculate
{
	int FirstNumber;
	int SecondNumber;
	unsigned short OperationType;
};

struct SC_Calculate
{
	long long Result;
};

#pragma pack(pop)

#endif // __PROTOCOL_H