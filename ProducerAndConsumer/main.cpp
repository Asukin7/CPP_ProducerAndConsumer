#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define N 10

int buffer[N] = { 0 };
int in = 0, out = 0;

HANDLE dmutex, dempty, dfull;

void gotoxy(int x, int y)
{
	COORD pos = { x,y };
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOut, pos);
}

void _printf(int n)
{
	gotoxy(6, n);
	switch (buffer[n])
	{
	case 1:
		printf("生产>|■■■■■■|     ");
		break;
	case 0:
		printf("     |            |>消费");
		break;
	default:
		break;
	}
	Sleep(170);
}

DWORD WINAPI producer(void *p)
{
	while (1)
	{
		WaitForSingleObject(dempty, INFINITE);
		WaitForSingleObject(dmutex, INFINITE);

		buffer[in] = 1;
		_printf(in);
		in = (in + 1) % N;

		ReleaseSemaphore(dmutex, 1, NULL);
		ReleaseSemaphore(dfull, 1, NULL);
	}
	return 0;
}

DWORD WINAPI consumer(void *p)
{
	while (1)
	{
		WaitForSingleObject(dfull, INFINITE);
		WaitForSingleObject(dmutex, INFINITE);

		buffer[out] = 0;
		_printf(out);
		out = (out + 1) % N;

		ReleaseSemaphore(dmutex, 1, NULL);
		ReleaseSemaphore(dempty, 1, NULL);
	}
	return 0;
}

int main()
{
	dmutex = CreateSemaphore(NULL, 1, 1, NULL);
	dempty = CreateSemaphore(NULL, N, N, NULL);
	dfull = CreateSemaphore(NULL, 0, N, NULL);

	HANDLE hdproducer[5];
	HANDLE hdconsumer[5];

	for (int i = 0; i < 10; i++)
	{
		printf("   %d       |            |\n", i);
	}

	for (int i = 0; i < 5; i++)
	{
		hdproducer[i] = CreateThread(NULL, 0, producer, NULL, 0, 0);
	}

	for (int i = 0; i < 5; i++)
	{
		hdconsumer[i] = CreateThread(NULL, 0, consumer, NULL, 0, 0);
	}

	WaitForMultipleObjects(5, hdproducer, TRUE, INFINITE);
	WaitForMultipleObjects(5, hdconsumer, TRUE, INFINITE);

	return 0;
}