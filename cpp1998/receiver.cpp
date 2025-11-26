
#include<iostream>
#include<fstream>
#include<string>
#include<windows.h>
#include<vector>
#ifdef max
#undef max
#endif
#include<limits>
using namespace std;
const int maxMessageLength = 20;
int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	string binFileName;
	cout << "Введите имя бинарного файла\n";
	cin >> binFileName;

	cout << "Введите Число процессов\n";
	int numberOfProcesses;



	cin >> numberOfProcesses;

	int numberOfMessages;
	cout << "Введите Число сообщений\n";
	cin >> numberOfMessages;

	fstream file(binFileName, ios::binary| ios::trunc | ios::out);
	int head = 0;
	int tail = 0;

	

	file.write((char*)&head, sizeof(int));
	file.write((char*)&tail, sizeof(int));
	file.write((char*)&numberOfMessages, sizeof(int));
	
	
	string empty(maxMessageLength, '\0');
	for (int i = 0; i < numberOfMessages; i++)
		file.write(empty.c_str(), maxMessageLength);

	file.close();


	HANDLE* CreationEvents = new HANDLE[numberOfProcesses];




	STARTUPINFO* siRec = new STARTUPINFO[numberOfProcesses];
	PROCESS_INFORMATION* piRec = new PROCESS_INFORMATION[numberOfProcesses];
	HANDLE hEmpty = CreateSemaphore(NULL, numberOfMessages, numberOfMessages, "Global\\QueueEmpty");
	HANDLE hFull = CreateSemaphore(NULL, 0, numberOfMessages, "Global\\QueueFull");
	HANDLE hMutex = CreateMutex(NULL, FALSE, "Global\\QueueMutex");
	

	for (int i = 0; i < numberOfProcesses; i++) {

		ZeroMemory(&siRec[i], sizeof(STARTUPINFO));
		siRec[i].cb = sizeof(STARTUPINFO);
		SECURITY_ATTRIBUTES sa;
		ZeroMemory(&sa, sizeof(sa));
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		

		CreationEvents[i] = CreateEvent(&sa, FALSE, FALSE, NULL);
		uintptr_t hValue = reinterpret_cast<uintptr_t>(CreationEvents[i]);
		string consoleArgs = "Sender.exe " + binFileName + " " + to_string(hValue);
		vector<char> cmd(consoleArgs.begin(), consoleArgs.end());
		cmd.push_back('\0');
		
		if (!CreateProcess(NULL, &cmd[0], NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &siRec[i], &piRec[i])) {
			cout << "Не удалось создать Sender\n";
		}


		
	





	}
	WaitForMultipleObjects(numberOfProcesses, CreationEvents, TRUE, INFINITE);
	
	

	while (true) {
		
		int operation = 0;
		cout << "Введите операцию:\n1:Чтение сообщения\n2:Выйти из программы\n";
		if (!(cin >> operation)) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			continue;
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		switch (operation) {
		case 1:
		{
			
			WaitForSingleObject(hFull, INFINITE);
			WaitForSingleObject(hMutex, INFINITE);
			
			file.open(binFileName, ios::binary | ios::in | ios::out);
			
			file.seekg(0, ios::beg);
			file.read((char*)&head, sizeof(int));
			
			
			char message[maxMessageLength];
			file.seekg(12 + head * maxMessageLength, ios::beg);
			file.read(message, maxMessageLength);

			
			cout << "Получено: " << message << endl;

			
			head = (head + 1) % numberOfMessages;
			file.seekp(0, ios::beg);
			file.write((char*)&head, sizeof(int));
			file.flush();
			file.close();

			ReleaseMutex(hMutex);        
			ReleaseSemaphore(hEmpty, 1, NULL);
			break;
		}
		case 2:
		{
			for (int i = 0; i < numberOfProcesses; i++) {
				CloseHandle(piRec[i].hProcess);
				CloseHandle(piRec[i].hThread);
				CloseHandle(CreationEvents[i]);
			}
			delete[] piRec;
			delete[] siRec;
			delete[] CreationEvents;
		
			exit(0);
			break;

		}
		default:
		{}
		}
	}
}
