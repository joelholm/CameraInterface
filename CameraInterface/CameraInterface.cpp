// CameraInterface.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
//
//void AddBackslashes(EdsChar *string) {
//	EdsChar curChar = string[0];
//	int place = 0;
//	//loop through string
//	while (curChar != '\0') {
//		//if a char matches find
//		if (curChar == '\') {
//			//replace it with replace
//			string[place] = replace;
//		}
//		curChar = string[++place];
//	}
//}


int main(int argc, char *argv[])
{
	cout << "Camera Interface" << endl;
	cout << "1 - Take Photo\t 2 - Photo Loop\t 3 - Exit\t" << endl;

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
		return -1;
	}
	
	int inVal = 0;
	cout << "$ ";
	cin >> inVal;
	while (inVal != 3) {
		if (inVal == 1) {
			mcc.TakePicture();
		}
		if (inVal == 2) {
			int numP = 0, milliseconds = 0;
			cout << "Number of photos ? ";
			cin >> numP;
			cout << "Every x milliseconds ? ";
			cin >> milliseconds;

			while (numP > 0) {
				time_t begin = clock();
				mcc.TakePicture();
				time_t end = clock();
				time_t diff = (end - begin) / (CLOCKS_PER_SEC / 1000);
				cout << "that took " << diff << " milliseconds" << endl;
				if (diff < milliseconds) {
					Sleep(milliseconds - diff);
				}
				numP--;
			}
		}
		cout << "$ ";
		cin >> inVal;
	}


	
	//Picture loop on timer

	//Maybe live view?


	/*for( int i = 0; i < argc; i++) {
		string cmdl = string(argv[i]);
		cout << "Argument[" << i << "]: " << cmdl.c_str() << endl;
	}*/

	//Command line arguments
	//1 -> Path to picture folder
	//2 -> 

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
