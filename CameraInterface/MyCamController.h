#pragma once

#define MAX_PATH_LENGTH 200

class MyCamController
{
public:
	int value;
	int badSetup = 0;
	int handlerStarted = 0;
	EdsChar path[MAX_PATH_LENGTH];
	EdsUInt32 numChildren = 0;

	EdsCameraListRef cameras;
	EdsCameraRef camera;

	MyCamController(char *exePath);
	~MyCamController();

	EdsObjectEventHandler hpt;
	int TakePicture();
	void GetFileName(EdsChar *out);
	void SetPath(EdsChar *path);
	void CharReplace(EdsChar *string, EdsChar find, EdsChar replace);
	int setProperties(EdsCameraRef camera);
	int EnterLiveView(int duration, int fps);
};

