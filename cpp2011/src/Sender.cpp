#include<iostream>
#include<fstream>
#include<string>
#include<windows.h>

#ifdef max
#undef max
#endif
#include<limits>
using namespace std;
const int maxMessageLength = 20;
int main(int argc, char* argv[])
{
	cout << argc;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	string binFileName( argv[1]);
	uintptr_t raw = stoull(argv[2]);
	HANDLE hEvent = reinterpret_cast<HANDLE>(raw);
	SetEvent(hEvent);


	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "Global\\QueueMutex");
	HANDLE hEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Global\\QueueEmpty");
	HANDLE hFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Global\\QueueFull");

	


	
		

	
	while (true) {
		int operation = 0;
		cout << "Введите операцию:\n1:Отправка сообщения\n2:Выйти из программы\n";
		if (!(cin >> operation)) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
			continue; 
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		switch (operation) {
		case 1:
		{
			WaitForSingleObject(hEmpty, INFINITE);
			WaitForSingleObject(hMutex, INFINITE);
			fstream binFile(binFileName, ios::binary | ios::in | ios::out);



			int tail;
			int capacity;
			char message[maxMessageLength];
			cin.getline(message, maxMessageLength);
			binFile.seekg(4, ios::beg);
			binFile.read((char*)&tail, sizeof(int));
			binFile.seekg(8, ios::beg);
			binFile.read((char*)&capacity, sizeof(int));
			binFile.seekp(12 + tail * maxMessageLength, ios::beg);
			binFile.write(message, maxMessageLength);
			
			


			tail = (tail + 1) % capacity;
			binFile.seekp(4);
			binFile.write((char*)&tail, sizeof(int));
			binFile.flush();

			binFile.close();

			ReleaseMutex(hMutex);
			ReleaseSemaphore(hFull, 1, NULL);




			break;
		}
		case 2:
		{

			exit(0);
			break;

		}
		default:
		{}

		}
	}
}