/*
Web Class Helper APIs Module Header File 2.1.0
This source code file is under MIT License.
Copyright (c) 2022 Class Tools Develop Team
Contributors: jsh-jsh ren-yc
*/
#ifndef APIS_H
#define APIS_H
#include "file-process.hpp"
#include "init.hpp"
#include "commands.hpp"
#include "functions.hpp"
#include "basic.hpp"

extern const wstring WCH_WDName[7];
extern map<wstring, function<void()>> WCH_command_support;
extern set<tuple<wstring, wstring, wstring>> WCH_settings_support;
extern vector<wstring> WCH_command_list;
extern set<tuple<int, int, wstring>> WCH_clock_list;
extern set<wstring> WCH_task_list;
extern set<pair<wstring, wstring>> WCH_work_list;
extern wstring WCH_window_title;
extern wstring WCH_command;
extern wstring WCH_ProgressBarStr;
extern HWND WCH_Win_hWnd;
extern HWND WCH_Tray_hWnd;
extern HMENU WCH_hMenu;
extern NOTIFYICONDATA WCH_NID;
extern ATL::CComPtr<ITaskbarList3> WCH_TBL;
extern Json::Value WCH_Settings;
extern int WCH_clock_num;
extern int WCH_task_num;
extern int WCH_work_num;
extern int WCH_clock_change;
extern int WCH_task_change;
extern int WCH_work_change;
extern int WCH_settings_change;
extern int WCH_ProgressBarTot;
extern int WCH_InputTimes;
extern bool WCH_cmd_line;
extern bool WCH_anti_idle;
extern bool WCH_count_down;
extern bool WCH_program_end;
extern bool WCH_pre_start;
extern ifstream fin;
extern wifstream wfin;
extern ofstream fout;
extern wofstream wfout;
extern Json::Reader JSON_Reader;
extern Json::StreamWriterBuilder JSON_SWB;
extern unique_ptr<Json::StreamWriter> JSON_SW;
WCH_Time WCH_GetTime();
void WCH_Sleep(int _ms);
void WCH_printlog(wstring _mode, wstring _info);
void WCH_read_settings();
void WCH_read();
void WCH_save_settings();
bool WCH_save_func(bool output);
size_t WCH_GetNumDigits(size_t _n);

void WCH_Sleep(int _ms) {
	// Sleep.
	while (_ms > 0 && !WCH_program_end) {
		sleep_for(milliseconds(100));
		_ms -= 100;
	}
}

void WCH_PrintColor(WORD _mode) {
	// Change console text color.
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _mode);
}

void WCH_PrintChar(size_t _times, wchar_t _c) {
	// Print characters.
	while (_times > 0 && !WCH_program_end) {
		_times--;
		wcout << _c;
	}
}

wstring StrToWstr(string str) {
	// Convert multiple byte string to wide string.
	wstring result;
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), buffer, len);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}

string WstrToStr(wstring wstr) {
	// Convert wide string to multiple byte string.
	string result;
	int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}

string UrlEncode(const string _in) {
	// Get URL encode result.
	string _res = "";
	for (size_t i = 0; i < _in.size(); i++) {
		if (isalnum((unsigned char)_in[i]) || _in[i] == '-' || _in[i] == '_' || _in[i] == '.' || _in[i] == '~') {
			_res += _in[i];
		} else if (_in[i] == ' ') {
			_res += "+";
		} else {
			char _tmp1 = (unsigned char)_in[i] >> 4;
			char _tmp2 = (unsigned char)_in[i] % 16;
			_res += '%';
			_res += _tmp1 > 9 ? _tmp1 + 55 : _tmp1 + 48;
			_res += _tmp2 > 9 ? _tmp2 + 55 : _tmp2 + 48;
		}
	}
	return _res;
}

vector<wstring> WCH_split(const wstring& _in) {
	// Split CLI string.
	vector<wstring> _res;
	wstring _tmp;
	bool _flag = false;
	for (size_t i = 0; i < _in.size(); i++) {
		if (_in[i] == L' ' && !_flag && i != 0) {
			if (_in[i - 1] != L'"') {
				_res.push_back(_tmp);
				_tmp = L"";
			}
		} else if (_in[i] == L'"') {
			if (_flag) {
				_res.push_back(_tmp);
				_tmp = L"";
				_flag = false;
			} else {
				_flag = true;
			}
		} else {
			_tmp.push_back(_in[i]);
		}
	}
	if (_tmp != L"") {
		_res.push_back(_tmp);
	}
	if (_flag || find(_res.begin(), _res.end(), L"") != _res.end()) {
		_res.clear();
		_res.push_back(L"Incorrect");
	}
	if (_res.size() != 0) {
		if (_res.size() != 1) {
			wstring _debug = L"Input command array: \"" + _res[0] + L"\", ";
			for (size_t i = 1; i < _res.size() - 1; i++) {
				_debug.append(L"\"" + _res[i] + L"\", ");
			}
			_debug.append(L"\"" + _res[_res.size() - 1] + L"\"");
			WCH_printlog(WCH_LOG_STATUS_INFO, _debug);
		} else {
			WCH_printlog(WCH_LOG_STATUS_INFO, L"Input command array: \"" + _res[0] + L"\"");
		}
	}
	return _res;
}

WCH_Time WCH_GetTime() {
	// Get current time and return a WCH_Time object.
	WCH_Time NowTime;
	time_t rawtime;
	struct tm ptminfo;
	time(&rawtime);
	localtime_s(&ptminfo, &rawtime);
	NowTime.Year = ptminfo.tm_year + 1900;
	NowTime.Month = ptminfo.tm_mon + 1;
	NowTime.Day = ptminfo.tm_mday;
	NowTime.Hour = ptminfo.tm_hour;
	NowTime.Minute = ptminfo.tm_min;
	NowTime.Second = ptminfo.tm_sec;
	return NowTime;
}

wstring WCH_GetCompileTime() {
	// Get program compile time.
	wstring spi = StrToWstr(__DATE__);
	map<wstring, int> mon;
	mon[L"Jan"] = 1;
	mon[L"Feb"] = 2;
	mon[L"Mar"] = 3;
	mon[L"Apr"] = 4;
	mon[L"May"] = 5;
	mon[L"Jun"] = 6;
	mon[L"Jul"] = 7;
	mon[L"Aug"] = 8;
	mon[L"Sep"] = 9;
	mon[L"Oct"] = 10;
	mon[L"Nov"] = 11;
	mon[L"Dec"] = 12;
	return format(L"{}/{:02}/{:02} {}", spi.substr(7, 4), mon[spi.substr(0, 3)], stoi(spi[4] == L' ' ? spi.substr(5, 1) : spi.substr(4, 2)), StrToWstr(__TIME__));
}

wstring WCH_GetUniIdent() {
	// Get unique identification. (Public IP)
	wstring _in, _res;
	URLDownloadToFileW(NULL, L"https://api.ipify.org", L"WCH_IDENT.tmp", 0, NULL);
	wfin.open(L"WCH_IDENT.tmp");
	wfin >> _in;
	wfin.close();
	DeleteFileW(L"WCH_IDENT.tmp");
	for (size_t i = 0; i < _in.size(); i++) {
		if (_in[i] != L'.') {
			_res.push_back(_in[i]);
		}
	}
	return L"11111" + to_wstring(stoll(_res) % 99991);
}

size_t WCH_GetWstrDisplaySize(wstring _in) {
	// Get display length of wide string.
	size_t _size = 0;
	for (size_t i = 0; i < _in.size(); i++) {
		if (iswascii(_in[i])) {
			_size++;
		} else {
			_size += 2;
		}
	}
	return _size;
}

void WCH_SetWindowStatus(bool flag) {
	// Set the window status by Windows API.
	ShowWindow(WCH_Win_hWnd, flag);
	WCH_cmd_line = flag;
	WCH_printlog(WCH_LOG_STATUS_INFO, format(L"\"CONSOLE\" argument \"STATUS\" was set to {}", (flag == true ? L"\"SHOW\"" : L"\"HIDE\"")));
}

void WCH_SetTrayStatus(bool flag) {
	// Set the tray status by Windows API.
	ShowWindow(FindWindowW(L"Shell_trayWnd", NULL), (flag == true ? SW_SHOW : SW_HIDE));
	WCH_printlog(WCH_LOG_STATUS_INFO, format(L"\"TRAY\" argument \"STATUS\" was set to {}", (flag == true ? L"\"SHOW\"" : L"\"HIDE\"")));
}

void WCH_ShowTaskBarError() {
	// Show error on task bar icon.
	WCH_TBL->SetProgressValue(WCH_Win_hWnd, 100, 100);
	WCH_TBL->SetProgressState(WCH_Win_hWnd, TBPF_ERROR);
	WCH_Sleep(1000);
	WCH_TBL->SetProgressState(WCH_Win_hWnd, TBPF_NOPROGRESS);
}

void WCH_PrintIncorrect() {
	// Print text for incorrect inputs.
	WCH_printlog(WCH_LOG_STATUS_WARN, L"Your input code is uncorrect, please check and try again");
	wcout << L"Your input code is uncorrect, please check and try again." << endl;
	thread T(WCH_ShowTaskBarError);
	T.detach();
}

void WCH_PrintNetworkErr() {
	// Print text for network errors.
	WCH_printlog(WCH_LOG_STATUS_WARN, L"An network error occurred, please check your network connection and try to update this program");
	wcout << L"An network error occurred, please check your network connection and try to update this program." << endl;
	thread T(WCH_ShowTaskBarError);
	T.detach();
}

void WCH_CheckHotkey() {
	// Check when hot key is pressed.
	if (!WCH_program_end) {
		if (WCH_anti_idle) {
			if (StrToWstr(WCH_Settings["AntiIdleEndPrompt"].asString()) == L"True") {
				if (MessageBoxW(NULL, StrToWstr(WCH_Settings["AntiIdleEndContent"].asString()).c_str(), L"WCH WARN", MB_ICONWARNING | MB_YESNO | MB_TOPMOST) == IDNO) {
					return;
				}
			}
			WCH_anti_idle = false;
			WCH_SetTrayStatus(true);
		}
		if (WCH_count_down) {
			WCH_count_down = false;
		}
		WCH_SetWindowStatus(true);
	}
}

void WCH_PutPicture() {
	// Press PrintScreen. (Keyboard)
	keybd_event(VK_SNAPSHOT, 0, 0, 0);
	keybd_event(VK_SNAPSHOT, 0, KEYEVENTF_KEYUP, 0);
	WCH_printlog(WCH_LOG_STATUS_INFO, L"Copying screenshot to clipboard");
}

void WCH_SaveImg() {
	// Save screenshot to file.
	WCH_Time now = WCH_GetTime();
	wstring SavePathDir = StrToWstr(WCH_Settings["ScreenshotSavePath"].asString());
	wstring SavePath = format(L"{}{:04}{:02}{:02}{:02}{:02}{:02}.jpg", SavePathDir, now.Year, now.Month, now.Day, now.Hour, now.Minute, now.Second);
	if (_waccess(SavePathDir.c_str(), 0) != 0) {
		CreateDirectoryW(SavePathDir.c_str(), NULL);
	}
	HDC hdcScreen = ::GetDC(NULL);
	double dDpi = (double)GetDeviceCaps(GetDC(GetDesktopWindow()), DESKTOPHORZRES) / GetSystemMetrics(SM_CXSCREEN);
	int nWidth = (int)round(GetDeviceCaps(hdcScreen, HORZRES) * dDpi);
	int nHeight = (int)round(GetDeviceCaps(hdcScreen, VERTRES) * dDpi);
	HDC hMemDC;
	HBITMAP hBitmap, hOldBitmap;
	hMemDC = CreateCompatibleDC(hdcScreen);
	hBitmap = CreateCompatibleBitmap(hdcScreen, nWidth, nHeight);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hdcScreen, 0, 0, SRCCOPY);
	GdiplusWrapper gdiplus {};
	gdiplus.SaveImage(hBitmap, SavePath.c_str(), L"image/jpeg");
	DeleteDC(hdcScreen);
	DeleteDC(hMemDC);
	DeleteObject(hBitmap);
	WCH_printlog(WCH_LOG_STATUS_INFO, L"Saving image to \"" + SavePath + L"\"");
}

void WCH_CheckAndDeleteFile(wstring _filename) {
	// Delete a file if it exists.
	if (_waccess(_filename.c_str(), 0) != -1) {
		DeleteFileW(_filename.c_str());
	}
}

bool WCH_FileIsBlank(wstring _filename) {
	// Check if a file is blank.
	if (_waccess(_filename.c_str(), 0) != -1) {
		wfin.open(_filename, ios::in);
		wstring _line;
		while (getline(wfin, _line)) {
			if (_line.size() != 0) {
				wfin.close();
				return false;
			}
		}
		wfin.close();
	}
	return true;
}

bool WCH_TaskKill(wstring name) {
	// Kill a task by system command.
	_wsystem((L"TASKKILL /F /IM " + name + L" > WCH_SYSTEM_NORMAL.tmp 2> WCH_SYSTEM_ERROR.tmp").c_str());
	bool _res = (!WCH_FileIsBlank(L"WCH_SYSTEM_NORMAL.tmp") && WCH_FileIsBlank(L"WCH_SYSTEM_ERROR.tmp"));
	DeleteFileW(L"WCH_SYSTEM_NORMAL.tmp");
	DeleteFileW(L"WCH_SYSTEM_ERROR.tmp");
	return _res;
}

bool WCH_CheckVersion(const WCH_Version _Fir, const WCH_Version _Sec) {
	if (_Fir.X < _Sec.X) {
		return true;
	} else if (_Fir.X > _Sec.X) {
		return false;
	} else {
		if (_Fir.Y < _Sec.Y) {
			return true;
		} else if (_Fir.Y > _Sec.Y) {
			return false;
		} else {
			if (_Fir.Z < _Sec.Z) {
				return true;
			} else {
				return false;
			}
		}
	}
}

WCH_Version WCH_GetVersion(wstring _in) {
	// Get version from string.
	WCH_Version _res;
	size_t _pos = _in.find(L".");
	if (_pos != wstring::npos) {
		_res.X = stoi(_in.substr(0, _pos));
		_in = _in.substr(_pos + 1);
	}
	_pos = _in.find(L".");
	if (_pos != wstring::npos) {
		_res.Y = stoi(_in.substr(0, _pos));
		_in = _in.substr(_pos + 1);
	}
	_res.Z = stoi(_in);
	return _res;
}

size_t WCH_GetNumDigits(size_t _n) {
	// Get digits of a number.
	size_t _cnt = 1;
	while ((_n /= 10) != 0) {
		_cnt++;
	}
	return _cnt;
}

void WCH_PrintProgressBar(int _sur, int _n, bool _flag) {
	// Print a progress bar.
	wstring _ETAStr = format(L"{:02}:{:02}:{:02}", (int)(_sur / 3600), (int)((_sur % 3600) / 60), (int)(_sur % 60));
	if (_flag) {
		wcout << "\r";
	}
	WCH_PrintColor(0x0A);
	for (int i = 0; i < _n / 2; i++) {
		wcout << WCH_ProgressBarStr;
	}
	WCH_PrintColor(0x0C);
	for (int i = _n / 2; i < 50; i++) {
		wcout << WCH_ProgressBarStr;
	}
	WCH_PrintColor(0x02);
	wcout << L" " << _n << L"%";
	WCH_PrintColor(0x07);
	wcout << L" ETA ";
	WCH_PrintColor(0x09);
	wcout << _ETAStr;
	WCH_PrintColor(0x07);
}

void WCH_ProgressBar() {
	// Progress bar.
	bool _cd = WCH_count_down;
	double _pro = 100.0 / WCH_ProgressBarTot;
	WCH_PrintProgressBar(WCH_ProgressBarTot, 0, false);
	WCH_TBL->SetProgressState(WCH_Win_hWnd, TBPF_NORMAL);
	WCH_TBL->SetProgressValue(WCH_Win_hWnd, 0, 100);
	for (int i = WCH_ProgressBarTot - 1; i > 0 && !WCH_program_end && !(_cd ^ WCH_count_down); i--) {
		WCH_Sleep(1000);
		WCH_PrintProgressBar(i, (int)((WCH_ProgressBarTot - i) * _pro), true);
		WCH_TBL->SetProgressValue(WCH_Win_hWnd, (unsigned long long)((WCH_ProgressBarTot - i) * _pro), 100);
	}
	WCH_Sleep(1000);
	WCH_PrintProgressBar(0, 100, true);
	WCH_TBL->SetProgressState(WCH_Win_hWnd, TBPF_NOPROGRESS);
	wcout << endl;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	// Window processing module.
	switch (message) {
		case WM_HOTKEY:
#ifdef _DEBUG
			WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Entering \"WndProc()\": \"WM_HOTKEY\" & \"wParam = " + to_wstring(wParam) + L"\" & \"lParam = " + to_wstring(lParam) + L"\"");
#endif
			if (wParam == WCH_HOTKEY_SHOW) {
				WCH_CheckHotkey();
			}
			break;
		case WM_CREATE:
#ifdef _DEBUG
			WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Entering \"WndProc()\": \"WM_CREATE\"");
#endif
			WCH_NID.cbSize = sizeof(WCH_NID);
			WCH_NID.hWnd = hWnd;
			WCH_NID.uID = 0;
			WCH_NID.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			WCH_NID.uCallbackMessage = WM_USER;
			WCH_NID.hIcon = (HICON)LoadImageW(NULL, L"WCH.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
			wcscpy(WCH_NID.szTip, WCH_window_title.c_str());
			Shell_NotifyIconW(NIM_ADD, &WCH_NID);
			WCH_hMenu = CreatePopupMenu();
			AppendMenuW(WCH_hMenu, MF_STRING, WCH_MENU_SHOW, L"Ctrl + Down");
			AppendMenuW(WCH_hMenu, MF_SEPARATOR, 0, NULL);
			AppendMenuW(WCH_hMenu, MF_STRING, WCH_MENU_QUIT, L"Quit");
			break;
		case WM_USER:
			if (lParam == WM_LBUTTONDOWN) {
#ifdef _DEBUG
				WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Entering \"WndProc()\": \"WM_USER\" & \"WM_LBUTTONDOWN\"");
#endif
				WCH_CheckHotkey();
			} else if (lParam == WM_RBUTTONDOWN) {
				POINT pt;
				int xx;
				GetCursorPos(&pt);
				SetForegroundWindow(hWnd);
				xx = TrackPopupMenu(WCH_hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd, NULL);
#ifdef _DEBUG
				WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Entering \"WndProc()\": \"WM_USER\" & \"WM_RBUTTONDOWN\" & \"xx = " + to_wstring(xx) + L"\"");
#endif
				if (xx == WCH_MENU_SHOW) {
#ifdef _DEBUG
					WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Entering \"WndProc()\": \"WM_USER\" & \"WM_RBUTTONDOWN\" & \"WCH_MENU_SHOW\"");
#endif
					WCH_CheckHotkey();
				} else if (xx == WCH_MENU_QUIT) {
#ifdef _DEBUG
					WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Entering \"WndProc()\": \"WM_USER\" & \"WM_RBUTTONDOWN\" & \"WCH_MENU_QUIT\"");
#endif
					WCH_command_list.clear();
					WCH_command_list.push_back(L"quit");
					wcout << endl;
					exit(0);
				} else if (xx == 0) {
					PostMessageW(hWnd, WM_LBUTTONDOWN, NULL, NULL);
				}
			}
			break;
		case WM_DESTROY:
#ifdef _DEBUG
			WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Entering \"WndProc()\": \"WM_DESTROY\"");
#endif
			Shell_NotifyIconW(NIM_DELETE, &WCH_NID);
			PostQuitMessage(0);
			break;
		default:
			if (message == RegisterWindowMessageW(L"TaskbarCreated")) {
				SendMessageW(hWnd, WM_CREATE, wParam, lParam);
			}
			break;
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

void WCH_ShowBugMessagebox(int errorcode, wstring errormsg) {
	// Show messagebox to inform a bug to user.
	wcout << L"\a";
	WCH_TBL->SetProgressState(WCH_Win_hWnd, TBPF_INDETERMINATE);
	if (MessageBoxW(NULL, (L"Oops! An error occurred.\nPlease inform our developers with the error message by opening a new Issue in our GitHub Repository.\nError message: " + to_wstring(errorcode) + L" " + errormsg + L"\nWould you like to visit the Issues page now?").c_str(), L"WCH ERROR", MB_ICONERROR | MB_YESNO) == IDYES) {
		_wsystem(L"START https://github.com/class-tools/Web-Class-Helper/issues/");
	}
	WCH_TBL->SetProgressState(WCH_Win_hWnd, TBPF_NOPROGRESS);
}

void WCH_signalHandler() {
	// Signal handler.
	signal(SIGINT, [](int signum) {
		WCH_command_list.clear();
		WCH_command_list.push_back(L"quit");
		wcout << endl;
		exit(signum - signum);
	});
	signal(SIGBREAK, [](int signum) {
		WCH_command_list.clear();
		WCH_command_list.push_back(L"quit");
		wcout << endl;
		exit(signum - signum);
	});
	signal(SIGABRT, [](int signum) {
		WCH_cmd_line = false;
		WCH_program_end = true;
		WCH_PrintColor(0x07);
		wcout << endl;
		WCH_printlog(WCH_LOG_STATUS_ERROR, L"Signal " + to_wstring(signum) + L" detected (Program aborted)");
		Sleep(500);
		WCH_SetWindowStatus(false);
		WCH_ShowBugMessagebox(signum, L"Program aborted");
		exit(signum);
	});
	signal(SIGFPE, [](int signum) {
		WCH_cmd_line = false;
		WCH_program_end = true;
		WCH_PrintColor(0x07);
		wcout << endl;
		WCH_printlog(WCH_LOG_STATUS_ERROR, L"Signal " + to_wstring(signum) + L" detected (Operation overflow)");
		Sleep(500);
		WCH_SetWindowStatus(false);
		WCH_ShowBugMessagebox(signum, L"Operation overflow");
		exit(signum);
	});
	signal(SIGILL, [](int signum) {
		WCH_cmd_line = false;
		WCH_program_end = true;
		WCH_PrintColor(0x07);
		wcout << endl;
		WCH_printlog(WCH_LOG_STATUS_ERROR, L"Signal " + to_wstring(signum) + L" detected (Illegal instruction)");
		Sleep(500);
		WCH_SetWindowStatus(false);
		WCH_ShowBugMessagebox(signum, L"Illegal instruction");
		exit(signum);
	});
	signal(SIGSEGV, [](int signum) {
		WCH_cmd_line = false;
		WCH_program_end = true;
		WCH_PrintColor(0x07);
		wcout << endl;
		WCH_printlog(WCH_LOG_STATUS_ERROR, L"Signal " + to_wstring(signum) + L" detected (Access to illegal memory)");
		Sleep(500);
		WCH_SetWindowStatus(false);
		WCH_ShowBugMessagebox(signum, L"Access to illegal memory");
		exit(signum);
	});
}

#endif