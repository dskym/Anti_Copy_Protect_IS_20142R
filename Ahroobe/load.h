#include <Windows.h>
#include <stdio.h>
#include <winsvc.h>
class Load_Driver {
private:
	SC_HANDLE   scm;         // SCManager 핸들 
	SC_HANDLE   svc;          // Service 핸들 
	HANDLE  hFile;               // sys파일 핸들 (device 핸들과 다른 것임)  
	wchar_t fullSysName[500];   // .sys파일 절대 경로 
	wchar_t sysName[100];       // .sys파일 이름 (.sys 앞 부분, first.sys면 first) 
	bool Load_Driver::OpenSCM();
	bool Load_Driver::GetDirectory(wchar_t *str);
	bool Load_Driver::GetDirectory(wchar_t *fullPath, wchar_t *sysName);
	bool Load_Driver::GetHandle(wchar_t *fullSysName);
public:
	Load_Driver(){};
	Load_Driver(wchar_t *sysName)                    { GetDirectory(sysName); }
	Load_Driver(wchar_t *fullPath, wchar_t *sysName) { GetDirectory(fullPath, sysName); }
	~Load_Driver() {
		CloseServiceHandle(svc);
		CloseServiceHandle(scm);
	}
	bool Load_Driver::InstallDriver();
	bool Load_Driver::StartDriver();
	bool Load_Driver::StopDriver();
	bool Load_Driver::RemoveDriver();
};
