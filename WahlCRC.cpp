// WahlCRC.cpp : Defines the entry point for the application.
//

#include "WahlCRC.h"

using namespace std;

namespace WahlCRC {

	
	// CRC Bits, Hamming Distance, Data Bytes, Polynomial
	CRC CRCs[TOTAL_CRCs] = {{16, 6, 16, {0, 0, 0x9e, 0xb2} },
							{32, 9, 27, {0x9d, 0x7f, 0x97, 0xd6} },
							{32, 6, 4092, {0x99, 0x60, 0x03, 0x4c} }};

	MessageCRC::MessageCRC(CRC _crc) {
		crc = _crc;
		memset(cstr, 0, MAX_MESSAGE_LENGTH);
		remainderBuffer = new unsigned char[crc.bits / 8];
		bufferN = crc.bits / 8;
		dataLength = 0;
	}

	MessageCRC::~MessageCRC() {
		
		if (remainderBuffer) {
			delete[] remainderBuffer;
		}
		
	}

	void MessageCRC::newCRC(CRC _crc) {
		crc = _crc;
		if (crc.bits / 8 >= bufferN) {
			delete[] remainderBuffer;
			remainderBuffer = new unsigned char[crc.bits / 8];
		}
	}

	void MessageCRC::findRemainder(std::string& message) {
		
		memcpy(cstr, message.c_str(), message.length());
		parser.newData(cstr, message.length(), crc.bits);
		dataLength = message.length();
		__findRemainder();

	}

	void MessageCRC::findRemainder(char* message, unsigned int N) {
		
		memcpy(cstr, message, N);
		parser.newData(cstr, N, crc.bits);
		dataLength = N;
		__findRemainder();

	}

	unsigned short charToNum(unsigned char byte) {
		unsigned char noob = byte;
		return (unsigned short)noob;
	}

	void MessageCRC::__findRemainder() {
		memset(remainderBuffer, 0, bufferN);
		unsigned char* parserBuffer = (unsigned char*)parser.getBuffer();
		unsigned int parserBufferLength = parser.getBufferLength();
		//int count = 0;
		// Are enough bits to pull
		while (true) {
			// Get bits until most sig a 1, or if eob, break
			//cout << count++ << ":" << endl;
			while (WahlBit::getMostSigBit(remainderBuffer[0]) != 128) {
				if (parser.isEndOfBits()) {
					return;
				}
				WahlBit::leftShiftBuffer((void*)remainderBuffer, bufferN, 1);
				parser.getBits(1);
				remainderBuffer[bufferN - 1] += parserBuffer[parserBufferLength - 1];
				//cout << "Buffer Bit: " << charToNum(parserBuffer[parserBufferLength - 1]) << ", Bit: " << parser.getBitLoc() << ", Byte: " << parser.getByteLoc() << endl;
				//cout << "EOB: " << parser.isEndOfBits() << endl;
				
			}

			//cout << "PreXOR: " << charToNum(remainderBuffer[0]) << ", " << charToNum(remainderBuffer[1]) << endl;


			// XOR
			for (int i = 0; i < bufferN; ++i) {\
				remainderBuffer[i] = remainderBuffer[i] ^ crc.poly[LARGEST_POLY_BYTES - bufferN + i];
			}

			//cout << "PostXOR: " << charToNum(remainderBuffer[0]) << ", " << charToNum(remainderBuffer[1]) << endl;
			//cout << endl;
		}
	}

	unsigned char* MessageCRC::getRemainder() {
		return remainderBuffer;
	}
	unsigned int MessageCRC::getRemainderN() {
		return bufferN;
	}
	CRC MessageCRC::getCRC() {
		return crc;
	}

}


