// WahlCRC.cpp : Defines the entry point for the application.
//

#include "WahlCRC.h"

using namespace std;

// Util functions from WahlBit
void __leftShiftBufferBytes(void* buffer, unsigned int const bufferLength, unsigned int const shift) {
	unsigned char* ptr = (unsigned char*)buffer;
	unsigned int byteShift = shift > bufferLength - 1 ? bufferLength - 1 : shift;

	for (int i = 0; i < bufferLength - byteShift; ++i) {
		ptr[i] = ptr[i + byteShift];
	}

	memset(ptr + bufferLength - byteShift, 0, byteShift);
}

void __leftShiftBufferBits(void* buffer, unsigned int const bufferLength, unsigned int const shift) {
	unsigned char* ptr = (unsigned char*)buffer;
	unsigned int bitShift = shift > 8 ? 8 : shift;

	unsigned int byte = 0;
	unsigned char nextByteBits = 0;
	for (unsigned int byte = 0; byte < bufferLength - 1; ++byte) {
		nextByteBits = 0;

		ptr[byte] = ptr[byte] << bitShift;

		nextByteBits = ptr[byte + 1] >> (8 - bitShift);
		ptr[byte] += nextByteBits;
	}
	ptr[bufferLength - 1] = ptr[bufferLength - 1] << bitShift;
}

void leftShiftBuffer(void* buffer, unsigned int const bufferLength, unsigned int const shift) {
	unsigned int byteShift = shift / 8;
	unsigned int bitShift = shift % 8;
	__leftShiftBufferBytes(buffer, bufferLength, byteShift);
	__leftShiftBufferBits(buffer, bufferLength, bitShift);
}

// The CRC Definitions
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
		parser.newData(cstr, message.length());
		dataLength = message.length();
		__findRemainder();

	}

	void MessageCRC::findRemainder(char* message, unsigned int N) {
		
		memcpy(cstr, message, N);
		parser.newData(cstr, N);
		dataLength = N;
		__findRemainder();

	}

	unsigned short charToNum(unsigned char byte) {
		unsigned char noob = byte;
		return (unsigned short)noob;
	}

	void MessageCRC::__findRemainder() {
		memset(remainderBuffer, 0, bufferN);
		//unsigned char* parserBuffer = (unsigned char*)parser.getBuffer();
		unsigned int parserBufferLength = crc.bits / 8 + 1;//parser.getBufferLength();
		//int count = 0;
		// Are enough bits to pull
		while (true) {
			// Get bits until most sig a 1, or if eob, break
			//cout << count++ << ":" << endl;
			while (remainderBuffer[0] < 128) {
				if (parser.EOB()) {
					return;
				}
				leftShiftBuffer((void*)remainderBuffer, bufferN, 1);
				//parser.getBits(1);
				remainderBuffer[bufferN - 1] += parser.get();
				//cout << "Buffer Bit: " << charToNum(parserBuffer[parserBufferLength - 1]) << endl; // ", Bit: " << parser.getBitLoc() << ", Byte: " << parser.getByteLoc() << endl;
				//cout << "EOB: " << parser.isEndOfBits() << endl;
				
			}

			//cout << "PreXOR: " << (charToNum(remainderBuffer[0]) << 8) + charToNum(remainderBuffer[1]) << endl;


			// XOR
			for (int i = 0; i < bufferN; ++i) {
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


