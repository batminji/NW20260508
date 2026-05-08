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

#define TOTAL_HEADER_SIZE 4

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

#endif // __PROTOCOL_H