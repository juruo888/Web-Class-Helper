/*
Web-Class-Helper File Process Module Header File 1.1.2
This source code file is under MIT License.
Copyright (c) 2022 Class Tools Develop Team
Contributors: jsh-jsh ren-yc
*/
#ifndef FILE_PROCESS_H
#define FILE_PROCESS_H
#include <bits/stdc++.h>
#include <windows.h>
#include <wininet.h>
#include <tlhelp32.h>
#include <conio.h>
#include <direct.h>
#include "commands.h"
#include "functions.h"
#include "apis.h"
#include "variables.h"
using namespace std;

extern const string Weekdayname[7];
extern multimap <int, pair <int, string>> WCH_clock;
extern set <string> WCH_task_list;
extern HWND hwnd;
extern int WCH_clock_num;
extern bool cmd_line;
extern bool anti_idle;
extern bool mode;
extern string op;
extern string UserName;
extern ifstream fin;
extern ofstream fout;
WCH_Time WCH_GetTime();
void WCH_Error(string INFO);

void WCH_printlog(int w, initializer_list <string> other) {
	// Print log.
	WCH_Time a = WCH_GetTime();
	string tt[21];
	char tmp[21];
	int pos = 0;
	sprintf(tmp, "[%02d:%02d:%02d]", a.Hour, a.Minute, a.Second);
	for (auto it = other.begin(); it != other.end(); it++) {
		if ((*it) == "r") {
			tt[pos++] = "Reading";
		} else if ((*it) == "w") {
			tt[pos++] = "Writing";
		} else if ((*it) == "s") {
			tt[pos++] = "Starting";
		} else if ((*it) == "e") {
			tt[pos++] = "Exiting";
		} else {
			tt[pos++] = (*it);
		}
	}
	freopen("./logs/latest.log", "a", stdout);
	if (w == WCH_LOG_MODE_ST) {
		printf("%s %s: %s Web-Class-Helper.\n", tmp, WCH_LOG_STATUS_INFO, tt[0].c_str());
	} else if (w == WCH_LOG_MODE_ERROR) {
		printf("%s %s: %s.\n", tmp, WCH_LOG_STATUS_ERROR, tt[0].c_str());
	} else if (w == WCH_LOG_MODE_RC) {
		printf("%s %s: Using command \"%s\".\n", tmp, WCH_LOG_STATUS_INFO, tt[0].c_str());
	} else if (w == WCH_LOG_MODE_RW) {
		printf("%s %s: Using command \"%s\".\n", tmp, WCH_LOG_STATUS_INFO, tt[0].c_str());
		printf("%s %s: %s file \"%s\".\n", tmp, WCH_LOG_STATUS_INFO, tt[0].c_str(), tt[1].c_str());
	} else if (w == WCH_LOG_MODE_KT) {
		printf("%s %s: %s task \"%s\".\n", tmp, WCH_LOG_STATUS_INFO, tt[1].c_str(), tt[0].c_str());
	}
	fclose(stdout);
	freopen("CON", "w", stdout);
}

void WCH_read_clock() {
	// Read clock data.
	WCH_Time q = WCH_GetTime();
	string NowWeekDay = Weekdayname[(q.Day + 2 * q.Month + 3 * (q.Month + 1) / 5 + q.Year + q.Year / 4 - q.Year / 100 + q.Year / 400 + 1) % 7];
	string FilePath = "./data/" + NowWeekDay + ".dat";
	WCH_printlog(WCH_LOG_MODE_RW, {"r", FilePath});
	fin.open(FilePath, ios::binary);
	if (!fin.is_open()) {
		return;
	}
	fin >> WCH_clock_num;
	for (int i = 1; i <= WCH_clock_num; i++) {
		int H, M;
		string Tname;
		fin >> H >> M >> Tname;
		WCH_clock.emplace(make_pair(H, make_pair(M, Tname)));
	}
	fin.close();
}

void WCH_read_task() {
	// Read task data.
	string FilePath = "./data/task.dat";
	WCH_printlog(WCH_LOG_MODE_RW, {"r", FilePath});
	fin.open(FilePath, ios::binary);
	if (!fin.is_open()) {
		return;
	}
	int list_size;
	fin >> list_size;
	for (int i = 1; i <= list_size; i++) {
		string TaskName;
		getline(fin, TaskName);
		WCH_task_list.insert(TaskName);
	}
}

void WCH_read() {
	// Read data.
	WCH_read_clock();
	WCH_read_task();
}

void WCH_save_clock() {
	// Save clock data.
	WCH_Time q = WCH_GetTime();
	string NowWeekDay = Weekdayname[(q.Day + 2 * q.Month + 3 * (q.Month + 1) / 5 + q.Year + q.Year / 4 - q.Year / 100 + q.Year / 400 + 1) % 7];
	string FilePath = "./data/" + NowWeekDay + ".dat";
	if (WCH_clock_num == 0) {
		if (access(FilePath.c_str(), 0) != -1) {
			DeleteFile(FilePath.c_str());
			WCH_printlog(WCH_LOG_MODE_RW, {"w", FilePath});
		} else {
			return;
		}
	}
	WCH_printlog(WCH_LOG_MODE_RW, {"w", FilePath});
	fout.open(FilePath, ios::binary);
	fout << WCH_clock_num << endl;
	for (auto it = WCH_clock.begin(); it != WCH_clock.end(); it++) {
		fout << (it -> first) << " " << (it -> second).first << " " << (it -> second).second << endl;
	}
	fout.close();
}

void WCH_save_task() {
	// Save task list data.
	string FilePath = "./data/task.dat";
	if (WCH_task_list.size() == 0) {
		if (access(FilePath.c_str(), 0) != -1) {
			DeleteFile(FilePath.c_str());
			WCH_printlog(WCH_LOG_MODE_RW, {"w", FilePath});
		} else {
			return;
		}
	}
	WCH_printlog(WCH_LOG_MODE_RW, {"w", FilePath});
	fout.open(FilePath, ios::binary);
	fout << WCH_task_list.size() << endl;
	for (auto it = WCH_task_list.begin(); it != WCH_task_list.end(); it++) {
		fout << (*it) << endl;
	}
	fout.close();
}

void WCH_save() {
	// Save data.
	WCH_save_clock();
	WCH_save_task();
	WCH_printlog(WCH_LOG_MODE_ST, {"e"});
}

void UTF8ToANSI(char *str) {
	// Convert UTF-8 to ANSI.
	int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
	WCHAR *wsz = new WCHAR[len + 1];
	len = MultiByteToWideChar(CP_UTF8, 0, str, -1, wsz, len);
	wsz[len] = 0;
	len = WideCharToMultiByte(CP_ACP, 0, wsz, -1, 0, 0, 0, 0);
	len = WideCharToMultiByte(CP_ACP, 0, wsz, -1, str, len, 0, 0);
	str[len] = 0;
}

string UTF8ToGB(const char* str) {
	string result;
	WCHAR *strSrc;
	LPSTR szRes;
	int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new CHAR[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);
	result = szRes;
	return result;
}

void WCH_RunSystem(string str) {
	freopen("WCH_SYSTEM.tmp", "w", stdout);
	system(str.c_str());
	freopen("CON", "w", stdout);
	Sleep(500);
	DeleteFile("WCH_SYSTEM.tmp");
}

#endif
