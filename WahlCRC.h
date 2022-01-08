// WahlCRC.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <string>
#include "WahlBitGetter.h"


#define TOTAL_CRCs 3
#define LARGEST_POLY_BYTES 4

#ifndef MAX_MESSAGE_LENGTH
#define MAX_MESSAGE_LENGTH 4092
#endif

// TODO: Reference additional headers your program requires here.

namespace WahlCRC {

	struct CRC {
		unsigned short bits;
		unsigned char hd;
		unsigned short dataBytes;
		unsigned char poly[LARGEST_POLY_BYTES];
	};

	extern CRC CRCs[TOTAL_CRCs];

	class MessageCRC {
	public:
		MessageCRC(CRC _crc);
		~MessageCRC();
		void newCRC(CRC _crc);

		void findRemainder(std::string& message);
		void findRemainder(char* message, unsigned int N);

		unsigned char* getRemainder();
		unsigned int getRemainderN();
		CRC getCRC();


	private:
		WahlBitGetter::BitGetter parser;
		CRC crc;
		char cstr[MAX_MESSAGE_LENGTH];
		unsigned char* remainderBuffer = nullptr;
		unsigned int bufferN;
		unsigned int dataLength;
		
		void __findRemainder();

	};
	

	
	
}
