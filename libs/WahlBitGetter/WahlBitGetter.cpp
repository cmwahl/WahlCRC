// WahlBitGetter.cpp : Defines the entry point for the application.
//

#include "WahlBitGetter.h"

using namespace std;

namespace WahlBitGetter {
	BitGetter::BitGetter(void* _data, unsigned int _N) {
		data = (unsigned char*)_data;
		N = _N;
	}

	char BitGetter::get() {

		//cout << "Byte: " << byteLoc << ", Bit: " << (short)bitLoc << endl;

		if (EOB()) {
			return -1;
		}

		currentByte = data[byteLoc];
		currentByte = currentByte << bitLoc;
		currentByte = currentByte >> 7;

		//cout << "current: " << (short)currentByte << endl;

		++bitLoc;
		byteLoc += bitLoc / 8;
		bitLoc = bitLoc % 8;

		return currentByte;
	}

	bool BitGetter::setByteLoc(unsigned int loc) {
		if (loc <= N) {
			byteLoc = loc;
			return true;
		}

		return false;
	}

	bool BitGetter::setBitLoc(unsigned char loc) {
		if (loc < 8) {
			bitLoc = loc;
			return true;
		}

		return false;
	}

	unsigned int BitGetter::getByteLoc() {
		return byteLoc;
	}

	unsigned char BitGetter::getBitLoc() {
		return bitLoc;
	}

	bool BitGetter::EOB() {
		return byteLoc == N;
	}

	void BitGetter::resetLoc() {
		bitLoc = 0;
		byteLoc = 0;
	}

	void BitGetter::newData(void* _data, unsigned int _N) {
		data = (unsigned char*)_data;
		N = _N;

		byteLoc = 0;
		bitLoc = 0;
	}
}

