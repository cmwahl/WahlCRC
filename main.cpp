
#include "WahlCRC.h"
#include <string>

using namespace std;

int main()
{
	
	unsigned char* message = new unsigned char[6];

	memset(message, 0, 6);
	message[0] = 'd';
	message[1] = '\0';
	message[2] = '\0';
	message[3] = '\0';
	message[4] = '\0';

	string messagestr = "dog  ";

	WahlCRC::MessageCRC crcer(WahlCRC::CRCs[2]);
	crcer.findRemainder((char*)message, 5);

	for (int i = 0; i < 4; ++i) {
		message[1 + i] = crcer.getRemainder()[i];
	}

	cout << "New characters: " << endl;
	for (int i = 0; i < crcer.getRemainderN(); ++i) {
		unsigned char noob = crcer.getRemainder()[i];
		unsigned short check = noob;
		cout << check << endl;
	}

	crcer.findRemainder((char*)message, 5);

	cout << "Check remainder: " << endl;
	for (int i = 0; i < crcer.getRemainderN(); ++i) {
		unsigned char noob = crcer.getRemainder()[i];
		unsigned short check = noob;
		cout << check << endl;
	}

	delete[] message;

	return 0;
}