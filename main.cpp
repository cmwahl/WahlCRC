
#include "WahlCRC.h"
#include <string>

using namespace std;

int main()
{
	
	unsigned char* message = new unsigned char[6];

	memset(message, 0, 6);
	message[0] = 'd';
	message[1] = 'o';
	message[2] = 'g';
	message[3] = '\0';
	message[4] = '\0';

	string messagestr = "dog  ";

	WahlCRC::MessageCRC crcer(WahlCRC::CRCs[0]);
	crcer.findRemainder((char*)message, 5);

	cout << "Pre: " << messagestr << endl;

	for (unsigned int i = 0; i < crcer.getRemainderN(); ++i) {
		messagestr[3 + i] = crcer.getRemainder()[i];
	}

	cout << "Post:" << messagestr << endl;
	cout << "New characters: " << endl;
	for (int i = 0; i < 2; ++i) {
		unsigned char noob = messagestr[3 + i];
		unsigned short check = noob;
		cout << check << endl;
	}

	delete[] message;

	return 0;
}