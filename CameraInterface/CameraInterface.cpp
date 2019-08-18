// CameraInterface.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

int main(int argc, char *argv[])
{
	cout << "Camera Interface" << endl;
	cout << "1 - Take Photo\t 2 - Live View\t" << endl;
	cout << "0 - Exit" << endl;

	EdsChar path[MAX_PATH_LENGTH];
	if ( argc > 1 ) {
		//Use the first parameter as the path to the picture folder
		//Example path "C:\\Users\joeld\Desktop\\"
		strcpy_s(path, argv[1]);
	}
	else {
		//otherwise use the executable's directory
		strcpy_s(path, argv[0]);
	}

	MyCamController mcc(path);

	if (mcc.badSetup) {
		Sleep(1000);
		return -1;
	}
	
	int inVal = -1;
	cout << "$ ";
	cin >> inVal;
	while (inVal != 0) {
		if (inVal == 1) {
			mcc.TakePicture();
		}
		if (inVal == 2) {
			int dur = 0, fps = 0;
			cout << "Duration in seconds ? ";
			cin >> dur;
			cout << "Frames per second ? ";
			cin >> fps;

			mcc.EnterLiveView(dur, fps);
		}
		cout << "$ ";
		cin >> inVal;
	}

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
