#include <Windows.h>
#include <stdio.h>
#include <winsvc.h>
class Load_Driver {
private:
	SC_HANDLE   scm;         // SCManager �ڵ� 
	SC_HANDLE   svc;          // Service �ڵ� 
	HANDLE  hFile;               // sys���� �ڵ� (device �ڵ�� �ٸ� ����)  
	wchar_t fullSysName[500];   // .sys���� ���� ��� 
	wchar_t sysName[100];       // .sys���� �̸� (.sys �� �κ�, first.sys�� first) 
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
