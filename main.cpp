
#include "WahlCRC.h"
#include <string>

using namespace std;

int main()
{
	
	unsigned char* message = new unsigned char[10];

	memset(message, 0, 10);
	message[0] = 'd';
	message[1] = 'o';
	message[2] = 'g';
	message[3] = 's';
	message[4] = '!';

	string messagestr = "dog  ";

	WahlCRC::MessageCRC crcer(WahlCRC::CRCs[0]);
	crcer.findRemainder((char*)message, 9);

	for (int i = 0; i < crcer.getCRC().bits/8; ++i) {
		message[9 - crcer.getRemainderN() + i] = crcer.getRemainder()[i];
	}

	unsigned long long int checkInt = 0;
	cout << "New characters: " << endl;
	for (int i = 0; i < crcer.getRemainderN(); ++i) {
		unsigned char noob = crcer.getRemainder()[i];
		unsigned short check = noob;
		cout << check << endl;
		checkInt = checkInt << 8;
		checkInt += check;
	}
	cout << "Int: " << checkInt << endl;

	crcer.findRemainder((char*)message, 9);

	
	cout << "Check remainder: " << endl;
	for (int i = 0; i < crcer.getRemainderN(); ++i) {
		unsigned char noob = crcer.getRemainder()[i];
		unsigned short check = noob;
		cout << check << endl;
		
	}
	

	delete[] message;

	return 0;
}