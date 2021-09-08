// WahlBit.cpp : Defines the entry point for the application.
//

#include "WahlBit.h"

namespace WahlBit {
	
	// Bit values
	unsigned char bitVals[8] = { 128, 64, 32, 16, 8 , 4, 2, 1 };

	// CLASS
	BitsParser::BitsParser() {

	}

	BitsParser::BitsParser(void* _data, unsigned int _dataLength, unsigned int maxNumBitsPerOperations) {
		data = (unsigned char*)_data;
		dataLength = _dataLength;
		bufferLength = maxNumBitsPerOperations / 8 + 1;
		buffer = (unsigned char*)malloc(bufferLength);

	}

	BitsParser::~BitsParser() {
		free(buffer);
	}
	
	void BitsParser::newData(void* _data, unsigned int _dataLength, unsigned int maxNumBitsPerOperations){
		
		data = (unsigned char*)_data;
		dataLength = _dataLength;
		
		if (bufferLength <= maxNumBitsPerOperations / 8 + 1) {
			bufferLength = maxNumBitsPerOperations / 8 + 1;
			free(buffer);
			buffer = (unsigned char*)malloc(bufferLength);
		}

		setBitLoc(0);
		setByteLoc(0);
			
		
	}

	void* BitsParser::getBits(unsigned int numberOfBits) {
		if (endOfBits) {
			return nullptr;
		}

		if (numberOfBits / 8 >= bufferLength - 1) {
			return false;
		}

		copyToBuffer();

		unsigned int shift = bufferLength * 8 - numberOfBits;

		if (bytesLoc + (bitsLoc + numberOfBits) / 8 >= dataLength) {
			endOfBits = true;
			return nullptr;
		}
		
		__leftShiftBufferBits((void*)buffer, bufferLength, bitsLoc);
		rightShiftBuffer((void*)buffer, bufferLength, shift);

		updateLocs(numberOfBits);

		return (void*)buffer;
	}

	bool BitsParser::putBits(void* bitString, unsigned int bitStringLength, unsigned int numBits) {
		if (endOfBits) {
			return false;
		}

		if (numBits / 8 >= bufferLength) {
			return false;
		}

		// Reset and Fill buffer then set all unwanted bits to 0
		memset(buffer, 0, bufferLength);
		memcpy(buffer + (bufferLength - bitStringLength), bitString, bitStringLength);

		unsigned int firstBufferIndex = bufferLength - (numBits / 8) - 1;
		buffer[firstBufferIndex] = buffer[firstBufferIndex] << (8 - numBits % 8);
		buffer[firstBufferIndex] = buffer[firstBufferIndex] >> (8 - numBits % 8);

		// Line up
		leftShiftBuffer(buffer, bufferLength, 8 - bitsLoc - (numBits % 8));

		// First byte
		unsigned int bitsLeft = numBits;
		unsigned char replacer = 255;
		unsigned int bitsInFirstByte = numBits > (8 - bitsLoc) ? (8 - bitsLoc) : numBits;
		for (int i = bitsLoc; i < bitsLoc + bitsInFirstByte; ++i) {
			replacer -= bitVals[i];
		}

		data[bytesLoc] = data[bytesLoc] & replacer;

		data[bytesLoc] = data[bytesLoc] | buffer[firstBufferIndex];

		bitsLeft -= bitsInFirstByte;

		// Middle bytes
		unsigned int lastByte = 0;
		for (unsigned int i = 1; i <= (bitsLeft / 8); ++i) {
			data[bytesLoc + i] = buffer[firstBufferIndex + i];
			bitsLeft -= 8;
			lastByte = bytesLoc + i + 1;
		}

		// Last byte
		if (bitsLeft) {
			replacer = 255;
			for (int i = 0; i < bitsLeft; ++i) {
				replacer -= bitVals[i];
			}
			data[lastByte] = data[lastByte] & replacer;
			data[lastByte] = data[lastByte] | buffer[bufferLength - 1];
		}

		setByteLoc(lastByte + 1);
		setBitLoc(bitsLeft);
	}

	bool BitsParser::copyToBuffer() {

		if (endOfBits) {
			return false;
		}

		memset(buffer, 0, bufferLength);

		unsigned int bytesLeft = dataLength - bytesLoc;
		unsigned int bytesToWrite = bytesLeft >= bufferLength ? bufferLength : bytesLeft;

		memcpy(buffer, data + bytesLoc, bytesToWrite);

		return true;
	}

	void BitsParser::updateLocs(unsigned int numberOfBits) {

		if (bytesLoc + (bitsLoc + numberOfBits) / 8 >= dataLength) {
			endOfBits = true;
			bitsLoc = 7;
			bytesLoc = dataLength - 1;
			return;
		}

		bytesLoc += (numberOfBits + bitsLoc) / 8;
		bitsLoc = (numberOfBits + bitsLoc) % 8;
	}

	void* BitsParser::getBuffer() {
		return (void*)buffer;
	}

	unsigned int BitsParser::getBufferLength() {
		return bufferLength;
	}

	unsigned int BitsParser::getByteLoc() {
		return bytesLoc;
	}

	unsigned int BitsParser::getBitLoc() {
		return bitsLoc;
	}

	bool BitsParser::setByteLoc(unsigned int _bytesLoc) {
		if (_bytesLoc >= dataLength) {
			return false;
		}
		bytesLoc = _bytesLoc;
		endOfBits = false;
		return true;
	}

	bool BitsParser::setBitLoc(unsigned short _bitsLoc) {
		if (_bitsLoc > 8) {
			return false;
		}
		bitsLoc = _bitsLoc;
		return true;
	}

	bool BitsParser::findBitString(void* _bitString, unsigned int bitStringLength, unsigned short numBits) {
		char* bitString = (char*)_bitString;
		unsigned int startByte = getByteLoc();
		unsigned int startBit = getBitLoc();

		if (endOfBits) {
			return false;
		}

		// If bad parameters
		if (numBits / 8 > bitStringLength || numBits / 8 > bufferLength) {
			return false;
		}

		// Get initial bits
		getBits(numBits);

		// Loop
		bool match = false;
		while (!match) {
			// Compare
			match = bitsCompare((void*)bitString, bitStringLength, (void*)buffer, bufferLength, numBits);

			if (match) {
				break;
			}
			// Get next bit (did not use premade methods due to optimizing just grabbing 1 bit)
			startBit += 1;
			startByte += startBit / 8;
			startBit = startBit % 8;

			if (startByte == dataLength) {
				endOfBits = true;
				return false;
			}
			
			__leftShiftBufferBits((void*)buffer, bufferLength, 1);

			// Clear unwanted bit
			buffer[bufferLength - numBits / 8 - 1] = buffer[bufferLength - (numBits / 8) - 1] << (8 - numBits);
			buffer[bufferLength - numBits / 8 - 1] = buffer[bufferLength - (numBits / 8) - 1] >> (8 - numBits);

			unsigned char addTo = (data[startByte + (startBit + numBits - 1) / 8] << (startBit + numBits - 1)%8); //This is wrong, don't need startBit, need the numBits away from startBit. Start a leastSig bit/byte
			addTo = addTo >> 7;
			buffer[bufferLength - 1] += addTo;
		}

		setBitLoc(startBit);
		setByteLoc(startByte);
		return match;
	}

	bool BitsParser::isEndOfBits() {
		return endOfBits;
	}


	// UTILITIES
	void rightShiftBuffer(void* buffer, unsigned int const bufferLength, unsigned int const shift) {
		unsigned int byteShift = shift / 8;
		unsigned int bitShift = shift % 8;
		//std::cout << "byteShift: " << byteShift << ", bitShift: " << bitShift << std::endl;
		__rightShiftBufferBytes(buffer, bufferLength, byteShift);
		__rightShiftBufferBits(buffer, bufferLength, bitShift);
	}

	void __rightShiftBufferBytes(void* buffer, unsigned int const bufferLength, unsigned int const shift) {
		unsigned char* ptr = (unsigned char*)buffer;
		unsigned int byteShift = shift > bufferLength - 1 ? bufferLength - 1 : shift;
		
		for(int i = 0; i < bufferLength - byteShift; ++i) {
			ptr[bufferLength - i - 1] = ptr[bufferLength - i - 1 - byteShift];
		}

		memset(ptr, 0, byteShift);
	}

	void __rightShiftBufferBits(void* buffer, unsigned int const bufferLength, unsigned int const shift) {
		unsigned char* ptr = (unsigned char*)buffer;
		unsigned int bitShift = shift > 8 ? 8 : shift;

		unsigned int byte = 0;
		unsigned char prevByteBits = 0;
		for(unsigned int i = 1; i < bufferLength; ++i) {
			byte = bufferLength - i;
			prevByteBits = 0;

			ptr[byte] = ptr[byte] >> bitShift;
			
			prevByteBits = ptr[byte - 1] << (8 - bitShift);
			ptr[byte] += prevByteBits;
		}
		ptr[0] = ptr[0] >> bitShift;
	}

	void leftShiftBuffer(void* buffer, unsigned int const bufferLength, unsigned int const shift) {
		unsigned int byteShift = shift / 8;
		unsigned int bitShift = shift % 8;
		__leftShiftBufferBytes(buffer, bufferLength, byteShift);
		__leftShiftBufferBits(buffer, bufferLength, bitShift);
	}

	void __leftShiftBufferBytes(void* buffer, unsigned int const bufferLength, unsigned int const shift) {
		unsigned char* ptr = (unsigned char*)buffer;
		unsigned int byteShift = shift > bufferLength - 1 ? bufferLength - 1 : shift;

		for(int i = 0; i < bufferLength - byteShift; ++i) {
			ptr[i] = ptr[i + byteShift];
		}

		memset(ptr + bufferLength - byteShift, 0, byteShift);
	}

	void __leftShiftBufferBits(void* buffer, unsigned int const bufferLength, unsigned int const shift) {
		unsigned char* ptr = (unsigned char*)buffer;
		unsigned int bitShift = shift > 8 ? 8 : shift;

		unsigned int byte = 0;
		unsigned char nextByteBits = 0;
		for(unsigned int byte = 0; byte < bufferLength - 1; ++byte) {
			nextByteBits = 0;

			ptr[byte] = ptr[byte] << bitShift;

			nextByteBits = ptr[byte + 1] >> (8 - bitShift);
			ptr[byte] += nextByteBits;
		}
		ptr[bufferLength - 1] = ptr[bufferLength - 1] << bitShift;
	}

	bool bitsCompare(void* _buffer, unsigned int bufferLength, void* _data, unsigned int dataLength, unsigned int bits) {
		char* buffer = (char*)_buffer;
		char* data = (char*)_data;
		bool match = true;

		unsigned char read = data[dataLength - 1];

		if (bits / 8 > bufferLength || bits / 8 > dataLength) {
			return false;
		}
		for (int i = 0; i < bits / 8 + 1; ++i) {
			unsigned char testBuffer = buffer[bufferLength - i - 1];
			unsigned char testData = data[dataLength - i - 1];
			if (i == bits / 8) {
				testBuffer = testBuffer << 8 - bits;
				testBuffer = testBuffer >> 8 - bits;

				testData = testData << 8 - bits;
				testData = testData >> 8 - bits;
			}
			if (testBuffer != testData) {
				match = false;
				break;
			}
		}
		return match;
	}

	unsigned char getMostSigBit(unsigned char byte) {
		unsigned char compare = 128;
		while (compare > byte) {
			compare = compare >> 1;
		}
		return compare;
	}

	void printBytes(void* _buffer, unsigned int bufferLength) {
		char* buffer = (char*)_buffer;

		for (int i = 0; i < bufferLength; ++i) {
			unsigned char val = buffer[i];
			std::cout << (unsigned short) val;
			if (i != bufferLength - 1) {
				std::cout << ", ";
			}
		}
		std::cout << std::endl;
	}
}
