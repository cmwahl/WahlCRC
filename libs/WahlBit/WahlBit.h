// WahlBit.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>

namespace WahlBit {

	class BitsParser {
	public:
		BitsParser();
		BitsParser(void* _data, unsigned int _dataLength, unsigned int _bufferLength);
		~BitsParser();
		void newData(void* _data, unsigned int _dataLength, unsigned int _bufferLength);

		// Get bits methods
		void* getBits(unsigned int numberOfBits);
		void* getBuffer();
		unsigned int getBufferLength();

		// Put bits methods
		bool putBits(void* bitString, unsigned int bitStringLength, unsigned int numBits);

		// Parser Location Util
		unsigned int getByteLoc();
		unsigned int getBitLoc();
		bool setByteLoc(unsigned int _bytesLoc);
		bool setBitLoc(unsigned short _bitsLoc);

		// Searches for the bitString from its given position and sets parser position to the bitString
		// Returns true if found, false if reaches endOfBits
		bool findBitString(void* bitString, unsigned int bitStringLength, unsigned short numBits);

		// End of Bits
		bool isEndOfBits();

	private:
		bool endOfBits = false;
		unsigned char* data = nullptr;
		unsigned int dataLength = 0;
		unsigned char* buffer = nullptr; // 8 Byte buffer
		unsigned int bufferLength = 0;

		unsigned int bytesLoc = 0;
		unsigned short bitsLoc = 0;

		bool copyToBuffer();
		void updateLocs(unsigned int numberOfBits);
	};

	void rightShiftBuffer(void* buffer, unsigned int const bufferLength, unsigned int const shift);
	void __rightShiftBufferBytes(void* buffer, unsigned int const bufferLength, unsigned int const shift);
	void __rightShiftBufferBits(void* buffer, unsigned int const bufferLength, unsigned int const shift);
	void leftShiftBuffer(void* buffer, unsigned int const bufferLength, unsigned int const shift);
	void __leftShiftBufferBytes(void* buffer, unsigned int const bufferLength, unsigned int const shift);
	void __leftShiftBufferBits(void* buffer, unsigned int const bufferLength, unsigned int const shift);

	bool bitsCompare(void* _buffer, unsigned int bufferLength, void* _data, unsigned int dataLength, unsigned int bits);
	unsigned char getMostSigBit(unsigned char byte);

	void printBytes(void* _buffer, unsigned int bufferLength);
}




