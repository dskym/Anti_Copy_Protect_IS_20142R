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


bool Load_Driver::InstallDriver()
{
	Load_Driver::OpenSCM();    // scm �ڵ� ��� 
	svc = CreateService(scm,    // ���� �ڵ� ��� 
		sysName,
		sysName,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		fullSysName,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);
	if (!svc) {
		if (GetLastError() == ERROR_SERVICE_EXISTS) {
			// ���񽺰� �����ϸ�, �����ϴ� ���� ���� 
			svc = OpenService(scm,
				sysName,
				SERVICE_ALL_ACCESS);
			if (!svc) { // �� ������ �ڵ� �ݱ� 
				printf("service is not opened.\n");
				CloseServiceHandle(scm);
				return false;
			}
			else printf("open success!\n");
		}
		else {  // ���� ������ ���� ���簡 �ƴ� ��� �ڵ� �ݱ� 
			printf("(%d)\n", GetLastError());
			printf("neither exist nor create.\n");
			CloseServiceHandle(scm);
			return false;
		}
	}
	else { printf("service create successfully!\n"); }
	return true;
}

bool Load_Driver::StartDriver()
{
	if (0 == StartService(svc, 0, NULL)) {
		if (ERROR_SERVICE_ALREADY_RUNNING == GetLastError()) {
			printf("service already starts..\n");
		}
		else {
			printf("(%d)\n", GetLastError());
			printf("service can't start.\n");
			return false;
		}
	}
	printf("service already starts or started.\n");

	return true;
}

bool Load_Driver::StopDriver()
{
	SERVICE_STATUS svcStatus;
	ControlService(svc, SERVICE_CONTROL_STOP, &svcStatus);
	if (svcStatus.dwCurrentState != 1) {
		printf("service can't stop.\n");
		return false;
	}
	else {
		printf("service stop.\n");
		return true;
	}
}

bool Load_Driver::RemoveDriver()
{
	if (!svc){
		printf("driver doesn't exist.\n");
		return false;
	}
	DeleteService(svc);
	printf("driver is deleted.\n");
	return true;
}

bool Load_Driver::OpenSCM()
{
	scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!scm) {
		printf("service manager creation failed.(%d)\n", GetLastError());
		return false;
	}
	return true;
}

bool Load_Driver::GetDirectory(wchar_t *sysName)
{
	wchar_t Directory[400];
	wsprintf(this->sysName, L"%s", sysName);
	GetCurrentDirectory(sizeof(Directory), Directory);
	wsprintf(fullSysName, L"%s\\%s.sys", Directory, sysName);
	// ���� ���� ���θ� Ȯ���ϱ� ���ؼ� ������ �����. 
	if (GetHandle(fullSysName) == false) { return false; }
	else { return true; }
}

bool Load_Driver::GetDirectory(wchar_t *fullPath, wchar_t *sysName)
{
	wcscpy_s(this->sysName, sysName);
	wcscpy_s(fullSysName, fullPath);

	// ���� ���� ���θ� Ȯ���ϱ� ���ؼ� ������ �����. 
	if (GetHandle(fullSysName) == false) { return false; }
	else { return true; }
}

bool Load_Driver::GetHandle(wchar_t *fullSysName)
{
	hFile = CreateFile(fullSysName,
		GENERIC_READ,
		0, 0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);

	if (hFile == HANDLE(-1)) {
		printf("sys doesn't exist.\n");
		return false;
	}
	else {
		printf("sys exist.\n");
		CloseHandle(hFile);
		return true;
	}
}

//int main(void)
//{
//	int menu = 0;
//	int i = 0, j = 0;
//	wchar_t SYSFILE[100] = L"InfoSec2";
//	// ����̹� �ε��ϱ� 
//	Load_Driver loader = Load_Driver(SYSFILE);
//	j = 1;
//	while (true){
//		printf("-----------------------\n");
//		printf("1. Install driver\n");
//		printf("2. Start driver\n");
//		printf("3. Stop driver\n");
//		printf("4. Delete driver\n");
//		printf("-----------------------\n");
//		printf("menu select : ");
//		scanf_s("%d", &menu);
//		switch (menu)
//		{
//		case 1:
//			if (i == 1 && j == 0){ i = 0; Load_Driver loader = Load_Driver(SYSFILE); }
//			loader.InstallDriver();
//			j = 0;
//			break;
//		case 2:
//			loader.StartDriver();
//			break;
//		case 3:
//			loader.StopDriver();
//			i = 1;
//			break;
//		case 4:
//			loader.RemoveDriver();
//			i = 1;
//			break;
//		default:
//			printf("\nYou selected wrong menu. Select menu again.\n");
//			break;
//		}
//	}
//	// ����̹� �ڵ� ��� 
//	//	HANDLE hDevice = CreateFile(L"\\\\.\\MyDevice",
//	//		GENERIC_READ | GENERIC_WRITE,
//	//		0,
//	//		NULL,
//	//		OPEN_EXISTING,
//	//		FILE_ATTRIBUTE_NORMAL,
//	//		NULL);
//	//	if (hDevice == ((HANDLE)-1)) {
//	//		printf("(%d)\n",(int)hDevice);
//	//		printf("device handle can't be gained.\n");
//	//		return false;
//	//	}
//	//	else { // ����̽� �ڵ� ��� ���� (������ �ڵ��� �� ���Ƿ� �׳� �ݴ´�.) 
//	//		printf("device handle is gained! : %d\n", hDevice);
//	//		CloseHandle(hDevice);
//	//	}
//	// ����̹� �����ϰ� �����ϱ� 
//	return 0;
//}