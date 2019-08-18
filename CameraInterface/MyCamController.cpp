#include "pch.h"
#include "MyCamController.h"

EdsError EDSCALLBACK handlePhotoTaken(EdsObjectEvent inEvent, EdsBaseRef inRef, EdsVoid *inContext) {

	MyCamController *mcc = (MyCamController*)inContext;

	cout << "Picture Taken" << endl;

	EdsError error = EDS_ERR_OK;
	EdsDirectoryItemInfo photoInfo;
	error = EdsGetDirectoryItemInfo(inRef, &photoInfo);
	if ( error != EDS_ERR_OK) { cout << "ERROR: Couldn't get directory info -- " << error << endl; }
	EdsStreamRef stream;
	EdsChar* fileName = new EdsChar[MAX_PATH_LENGTH];
	mcc->GetFileName(fileName);

	cout << "Picture: " << fileName << endl;
	error = EdsCreateFileStream(fileName, kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, &stream);
	if (error != EDS_ERR_OK) { cout << "ERROR: Couldn't create a new file stream -- " << error << endl; }
	error = EdsDownload(inRef, photoInfo.size, stream);
	if (error != EDS_ERR_OK) { cout << "ERROR: Couldn't download picture into file -- " << error << endl; }
	EdsDownloadComplete(inRef);
	EdsRelease(stream);

	mcc->handlerStarted = 1;

	return EDS_ERR_OK;
}

MyCamController::MyCamController(char *exePath)
{
	EdsInitializeSDK();
	//Getting the first connected camera
	if (EdsGetCameraList(&cameras) != EDS_ERR_OK) {
		cout << "ERROR: didn't recieve list of cameras" << endl;
		badSetup = 1;
	}
	if (EdsGetChildCount(cameras, &numChildren) != EDS_ERR_OK) {
		cout << "ERROR: didn't recieve child count" << endl;
		badSetup = 1;
	}
	if (numChildren < 1) {
		cout << "ERROR: No camera found" << endl;
		badSetup = 1;
	}
	if (EdsGetChildAtIndex(cameras, 0, &camera) != EDS_ERR_OK) {
		cout << "ERROR: Couldn't get first camera" << endl;
		badSetup = 1;
	}

	if (EdsOpenSession(camera) != EDS_ERR_OK) {
		cout << "ERROR: Couldn't open session with camera" << endl;
		badSetup = 1;
	}
	
	//Set up picture callback function
	hpt = handlePhotoTaken;
	if (EdsSetObjectEventHandler(camera, kEdsObjectEvent_All, hpt, this) != EDS_ERR_OK) {
		cout << "ERROR: Couldn't set callback" << endl;
		badSetup = 1;
	}

	setProperties(camera);

	//Set path
	SetPath(exePath);

}

MyCamController::~MyCamController()
{
	EdsCloseSession(camera);
	EdsTerminateSDK();
}

int MyCamController::TakePicture() {
	EdsError error = EDS_ERR_OK;
	error = EdsSendCommand(camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_Completely);
	error += EdsSendCommand(camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_OFF);
	
	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL) && handlerStarted == 0 && error == EDS_ERR_OK)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	handlerStarted = 0;

	return 0;
}

std::wstring StringToWString(const std::string& s)
{
	std::wstring temp(s.length(), L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}

//Take path from command line argument.  With that info create a folder in that same directory 
//create a folder Pictures, if it doesn't exist.
void MyCamController::SetPath(EdsChar *path_p) {
	string s_fullPath(path_p);
	size_t locAt = s_fullPath.find_last_of('\\') + 1;
	string s_justPath = s_fullPath.substr(0, locAt);
	strcpy_s(path, MAX_PATH_LENGTH, s_justPath.c_str());
	s_justPath += "Pictures";
	wstring newDirectory = StringToWString(s_justPath);
	//Creating Pictures directories
	CreateDirectory(newDirectory.c_str(), NULL);
}


void MyCamController::GetFileName(EdsChar *out) {
	char dt[MAX_PATH_LENGTH];
	strcpy_s(out, MAX_PATH_LENGTH, path);
	strcat_s(out, MAX_PATH_LENGTH, "Pictures\\");
	time_t now = time(0);
	ctime_s(dt, MAX_PATH_LENGTH, &now);
	CharReplace(dt, '\n', '.');
	strcat_s(dt, MAX_PATH_LENGTH, "JPG");
	CharReplace(dt, ' ', '_');
	CharReplace(dt, ':', '-');
	strcat_s(out, MAX_PATH_LENGTH, dt);
}

void MyCamController::CharReplace(EdsChar *string, EdsChar find, EdsChar replace) {
	EdsChar curChar = string[0];
	int place = 0;
	//loop through string
	while (curChar != '\0') {
		//if a char matches find
		if (curChar == find) {
			//replace it with replace
			string[place] = replace;
		}
		curChar = string[++place];
	}
}

int MyCamController::setProperties(EdsCameraRef camera) {
	EdsError error = EDS_ERR_OK;
	// set the saveto property
	EdsUInt32 size, inData = kEdsSaveTo_Host;
	EdsDataType type;
	error += EdsGetPropertySize(camera, kEdsPropID_SaveTo, 0, &type, &size);
	error += EdsSetPropertyData(camera, kEdsPropID_SaveTo, 0, size, &inData);
	//Set Capacity
	EdsCapacity capacity = { 0x7FFFFFFF, 0x1000, 1 };
	error += EdsSetCapacity(camera, capacity);
	if (error != EDS_ERR_OK) {
		cout << "Error setting properites" << endl;
		return -1;
	}
	return 0;
}

int MyCamController::EnterLiveView(int duration, int fps) {
	//Live view setup
	EdsError error = EDS_ERR_OK;
	EdsUInt32 device; 
	error = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	device |= kEdsEvfOutputDevice_PC;
	error = EdsSetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);

	//Link live view to a file
	EdsStreamRef stream;
	EdsEvfImageRef evfPic;
	EdsChar* fileName = new EdsChar[MAX_PATH_LENGTH];
	GetFileName(fileName);
	error = EdsCreateFileStream(fileName, kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, &stream);
	if (error != EDS_ERR_OK) { cout << "ERROR: Couldn't create the file stream -- " << error << endl; }
	error = EdsCreateEvfImageRef(stream, &evfPic);
	if (error != EDS_ERR_OK) { cout << "ERROR: Couldn't create the live image ref -- " << error << endl; }

	cout << "Live view: " << fileName << endl;

	int in = 1;
	time_t begin = clock();
	time_t prev = begin;
	int diff = 0;
	while (diff < duration) {
		error = EdsDownloadEvfImage(camera, evfPic);
		if (error != EDS_ERR_OK) { cout << "ERROR: Couldn't download the live image -- " << error << endl; }

		time_t now = clock();
		int nowPrevDiff = (now - prev) / (CLOCKS_PER_SEC / 1000);
		cout << "space - " << nowPrevDiff << endl;
		if (nowPrevDiff < ( 1000 / fps )) {
			Sleep( (1000 / fps) - nowPrevDiff);
		}
		prev = now;
		diff = (now - begin) / (CLOCKS_PER_SEC);
	}
	

	//Live view deconstruction
	error = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	device &= ~kEdsEvfOutputDevice_PC;
	error = EdsSetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);

	EdsRelease(evfPic);
	EdsRelease(stream);
	return 0;
}