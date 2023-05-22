#include <iostream>
#include <graphics.h>
#include <fstream>
#include <windows.h>
#include<sstream>

using namespace std;

#define BUF_SIZE 8192

//newfile 123.txt 111111 cmyisniubi
//copy C:\Users\hikari\Desktop\789.txt /cmy host 
//write /cmy/123.txt 123456

/*窗口函数*/
void LogIn();
void SysUserInteractiveUI();
void ComUserInteractiveUI();
void ShowError();

/*其他函数*/
void ShareMemoryClear(LPVOID& ShareMemoryPointer);//清空共享内存
void MemoryInfoCpy(char* Buffer, LPVOID& ShareMemoryPointer);//将共享内存中的内容拷贝出来


/*指令空间，用于客户端向服务器传输指令*/
	//创建共享文件句柄
HANDLE InstructionSpace = CreateFileMapping(
	INVALID_HANDLE_VALUE,   // 物理文件句柄
	NULL,   // 默认安全级别
	PAGE_READWRITE,   // 可读可写
	0,   // 高位文件大小
	BUF_SIZE,   // 地位文件大小
	"InstructionSpace"   // 共享内存名称
);

//映射缓存区视图，得到指向共享内存的指针
LPVOID InstructionSend = MapViewOfFile(
	InstructionSpace,            // 共享内存的句柄
	FILE_MAP_ALL_ACCESS, // 可读写许可
	0,
	0,
	BUF_SIZE
);

/*权限空间，用于告知服务器用户的身份*/
HANDLE AccSpace = CreateFileMapping(
	INVALID_HANDLE_VALUE,   // 物理文件句柄
	NULL,   // 默认安全级别
	PAGE_READWRITE,   // 可读可写
	0,   // 高位文件大小
	BUF_SIZE,   // 地位文件大小
	"AccSpace"   // 共享内存名称
);

//映射缓存区视图，得到指向共享内存的指针
LPVOID AccSend = MapViewOfFile(
	AccSpace,            // 共享内存的句柄
	FILE_MAP_ALL_ACCESS, // 可读写许可
	0,
	0,
	BUF_SIZE
);


int main()
{
	LogIn();
    return 0;
}

void LogIn() 
{
begin:
	ExMessage m;//定义鼠标变量
	/*初始化*/
	initgraph(350, 250); //定义画布大小，宽度，长度
	setbkcolor(WHITE);//设置背景颜色
	cleardevice();//刷新
	char account[20] = {};
	char password[15] = {};
	fstream f;

	/*设置框和框中的信息*/
	setlinecolor(BLACK);
	rectangle(100, 78, 240, 98);//账号框
	rectangle(100, 128, 240, 148);//密码框
	rectangle(60, 178, 290, 208);//登录框
	rectangle(250, 78, 290, 98);//账号输入框
	rectangle(250, 128, 290, 148);//密码输入框
	RECT r1 = { 60,178,290,208 };
	settextcolor(BLACK);
	drawtext(("登录"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//登录文本
	RECT r2 = { 250,78,290,98 };
	settextcolor(BLACK);
	drawtext(("输入"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//账号输入
	RECT r3 = { 250,128,290,148 };
	settextcolor(BLACK);
	drawtext(("输入"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//密码输入

	/*设置文本*/
	settextstyle(16, 0, ("宋体"));//设置文字大小 格式 
	setbkmode(TRANSPARENT);// 去掉文字背景
	//下面是输出字体 参数：（起始位置x 起始位置y “带输出的字符串”）
	settextcolor(BLACK);
	outtextxy(60, 80, ("账号"));
	outtextxy(60, 130, ("密码"));
	settextstyle(12, 0, ("宋体"));
	settextstyle(16, 0, ("楷体"));
	outtextxy(73, 30, ("欢迎进入hikari文件系统！"));
	

	/*按钮设定*/
	
	while (1) {
		m = getmessage(EM_MOUSE | EM_KEY);
		if (m.x >= 60 && m.y >= 178 && m.x <= 290 && m.y <= 208) {//登录键
			if (m.message == WM_LBUTTONUP) {//左键弹起时
				if (account[0] == '1' && account[1] == '2' && account[2] == '3' && account[3] == '\0'&& password[0] == '1' && password[1] == '2' && password[2] == '3' && password[3] == '\0') {//系统用户
					SysUserInteractiveUI();//交互界面
					goto begin;
				}
				else if (account[0] == '3' && account[1] == '2' && account[2] == '1' && account[3] == '\0' && password[0] == '3' && password[1] == '2' && password[2] == '1' && password[3] == '\0') {//普通用户
					ComUserInteractiveUI();//交互界面
					goto begin;
				}
				else {
					ShowError();
					goto begin;
				}
			}
		}
		else if (m.x >= 250 && m.y >= 78 && m.x <= 290 && m.y <= 98) {//账号输入
			if (m.message == WM_LBUTTONUP) {//左键弹起时
				InputBox(account, 20, "请输入你的账号");
				clearrectangle(101, 79, 239, 97);
				outtextxy(100, 78, account);
			}
		}
		else if (m.x >= 250 && m.y >= 128 && m.x <= 290 && m.y <= 148) {//密码输入
			if (m.message == WM_LBUTTONUP) {//左键弹起时
				InputBox(password, 15, "请输入你的密码");
				clearrectangle(101, 129, 239, 147);
				for (int i = 0; i < strlen(password); i++) {
					outtextxy(100 + i * 8, 128, L'*');
				}
			}
		}
	}
	return;
}

void ShowError() 
{
	settextstyle(12, 0, "宋体");
	settextcolor(RED);
	outtextxy(200, 157, "账号或密码错误！");
	settextcolor(BLACK);
	Sleep(300);
	clearrectangle(200, 157, 300, 170);
}

void SysUserInteractiveUI()
{
begin:
	ExMessage m;//定义鼠标变量
	/*初始化*/
	initgraph(550, 350); //定义画布大小，宽度，长度
	setbkcolor(WHITE);//设置背景颜色
	cleardevice();//刷新
	char instruction[BUF_SIZE] = {};
	char password[15] = {};
	fstream f;

	/*设置框和框中的信息*/
	setlinecolor(BLACK);
	rectangle(15, 40, 535, 270);//信息框
	rectangle(15, 310, 450, 330);//输入文本框
	rectangle(465, 15, 535, 35);//返回框
	rectangle(465, 280, 535, 300);//输入框
	rectangle(465, 310, 535, 330);//确认框
	RECT r1 = { 465,280, 535, 300 };
	settextcolor(BLACK);
	drawtext(_T("输入"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//输入指令
	RECT r2 = { 465, 310, 535, 330 };
	settextcolor(BLACK);
	drawtext(_T("确认"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//确认指令
	RECT r3 = { 465, 15, 535, 35 };
	settextcolor(BLACK);
	drawtext(_T("返回"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//返回上一层

	/*设置文本*/
	settextstyle(16, 0, "宋体");//设置文字大小 格式 
	setbkmode(TRANSPARENT);// 去掉文字背景
	//下面是输出字体 参数：（起始位置x 起始位置y “带输出的字符串”）
	settextcolor(BLACK);
	settextstyle(16, 0, "楷体");//设置文字大小 格式 
	outtextxy(15, 15, "当前目录:");
	outtextxy(90, 15, ("/cmy"));
	outtextxy(195, 285, "请输入指令");

	/*按钮设定*/

begin2:
	while (1) {
		m = getmessage(EM_MOUSE | EM_KEY);
		if (m.x >= 465 && m.y >= 280 && m.x <= 535 && m.y <= 300) {//指令输入
			if (m.message == WM_LBUTTONUP) {//左键弹起时
				if (m.message == WM_LBUTTONUP) {//左键弹起时
					InputBox(instruction, BUF_SIZE, "请输入指令");
					clearrectangle(17, 312, 448, 328);
					outtextxy(17, 312, instruction);
				}
			}
		}
		else if (m.x >= 465 && m.y >= 310 && m.x <= 535 && m.y <= 330) {//确认
			if (m.message == WM_LBUTTONUP) {//左键弹起时
				clearrectangle(16, 41, 534, 269);//清空信息栏
				char ReplyInfoRecBuffer[BUF_SIZE] = { 0 };//接受回复信息的数组

				
				//将数据拷贝到共享内存
				strcpy_s((char*)InstructionSend, strlen(instruction) + 1, instruction);
				strcpy_s((char*)AccSend, strlen("0") + 1, "0");

				/*对于write是否占用的判断*/
				HANDLE WriteSpace = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, "WriteSpace");//不断地从回复信息空间中获取数据
				LPVOID WriteRec = MapViewOfFile(WriteSpace, FILE_MAP_ALL_ACCESS, 0, 0, 0);
				istringstream InputSS(instruction);
				string WritingFile;
				if (WriteRec)
				{
					WritingFile = string((char*)WriteRec);
				}
				else
				{
					WritingFile = "";
				}
				
				string InputInstruction;
				string InputFile;
				InputSS >> InputInstruction;
				InputSS >> InputFile;
				if (InputFile == WritingFile && InputInstruction == "write")
				{
					clearrectangle(90, 15, 200, 30);
					outtextxy(90, 15, ReplyInfoRecBuffer);
					outtextxy(16, 41, "当前文件正在被其他用户写入，请稍后再试！");
					char NULLChar[] = "";//清空输入
					strcpy_s((char*)InstructionSend, strlen(NULLChar) + 1, NULLChar);
					goto begin2;
				}

				while (1)
				{
					HANDLE ReplyInfoSpace = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, "ReplyInfoSpace");//不断地从回复信息空间中获取数据
					LPVOID ReplyInfoRec = MapViewOfFile(ReplyInfoSpace, FILE_MAP_ALL_ACCESS, 0, 0, 0);
					if (!ReplyInfoRec)continue;
					if (strlen((char*)ReplyInfoRec))
					{
						MemoryInfoCpy(ReplyInfoRecBuffer, ReplyInfoRec);
						break;//如果获取到的数据，break
					}
				}

				clearrectangle(17, 42, 533, 268);//清空输入文本框

				/*输出返回的信息到信息框*/
				if (ReplyInfoRecBuffer[0] == '/')
				{
					clearrectangle(90, 15, 200, 30);
					outtextxy(90, 15, ReplyInfoRecBuffer);
					outtextxy(16, 41, "跳转成功！");
					continue;
				}
				int CountChangeLine = 1;
				string InputStr(ReplyInfoRecBuffer);
				for (int i = 0; i < InputStr.length(); i++)
				{
					if (InputStr[i] == '\n')
					{
						InputStr[i] = ' ';
						CountChangeLine++;
					}
				}
				istringstream InputSStr(InputStr);
				for (int i = 0; i < CountChangeLine; i++)
				{
					InputSStr >> InputStr;
					outtextxy(16, 41 + 18 * i, InputStr.c_str());
				}
			}
		}
		else if (m.x >= 465 && m.y >= 15 && m.x <= 535 && m.y <= 35) {//返回
			if (m.message == WM_LBUTTONUP) {//左键弹起时
				return;
			}
		}
	}

	/*解除指令空间文件映射*/
	UnmapViewOfFile(InstructionSend);
	CloseHandle(InstructionSpace);

	return;
}

void ComUserInteractiveUI()
{
begin:
	ExMessage m;//定义鼠标变量
	/*初始化*/
	initgraph(550, 350); //定义画布大小，宽度，长度
	setbkcolor(WHITE);//设置背景颜色
	cleardevice();//刷新
	char instruction[BUF_SIZE] = {};
	char password[15] = {};
	fstream f;

	/*设置框和框中的信息*/
	setlinecolor(BLACK);
	rectangle(15, 40, 535, 270);//信息框
	rectangle(15, 310, 450, 330);//输入文本框
	rectangle(465, 15, 535, 35);//返回框
	rectangle(465, 280, 535, 300);//输入框
	rectangle(465, 310, 535, 330);//确认框
	RECT r1 = { 465,280, 535, 300 };
	settextcolor(BLACK);
	drawtext(_T("输入"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//输入指令
	RECT r2 = { 465, 310, 535, 330 };
	settextcolor(BLACK);
	drawtext(_T("确认"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//确认指令
	RECT r3 = { 465, 15, 535, 35 };
	settextcolor(BLACK);
	drawtext(_T("返回"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//返回上一层

	/*设置文本*/
	settextstyle(16, 0, "宋体");//设置文字大小 格式 
	setbkmode(TRANSPARENT);// 去掉文字背景
	//下面是输出字体 参数：（起始位置x 起始位置y “带输出的字符串”）
	settextcolor(BLACK);
	settextstyle(16, 0, "楷体");//设置文字大小 格式 
	outtextxy(15, 15, "当前目录:");
	outtextxy(90, 15, ("/cmy"));
	outtextxy(195, 285, "请输入指令");

	/*按钮设定*/

begin2:
	while (1) {
		m = getmessage(EM_MOUSE | EM_KEY);
		if (m.x >= 465 && m.y >= 280 && m.x <= 535 && m.y <= 300) {//指令输入
			if (m.message == WM_LBUTTONUP) {//左键弹起时
				if (m.message == WM_LBUTTONUP) {//左键弹起时
					InputBox(instruction, BUF_SIZE, "请输入指令");
					clearrectangle(17, 312, 448, 328);
					outtextxy(17, 312, instruction);
				}
			}
		}
		else if (m.x >= 465 && m.y >= 310 && m.x <= 535 && m.y <= 330) {//确认
			if (m.message == WM_LBUTTONUP) {//左键弹起时
				clearrectangle(16, 41, 534, 269);//清空信息栏
				char ReplyInfoRecBuffer[BUF_SIZE] = { 0 };//接受回复信息的数组


				//将数据拷贝到共享内存
				strcpy_s((char*)InstructionSend, strlen(instruction) + 1, instruction);
				strcpy_s((char*)AccSend, strlen("1") + 1, "1");

				/*对于write是否占用的判断*/
				HANDLE WriteSpace = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, "WriteSpace");//不断地从回复信息空间中获取数据
				LPVOID WriteRec = MapViewOfFile(WriteSpace, FILE_MAP_ALL_ACCESS, 0, 0, 0);
				istringstream InputSS(instruction);
				string WritingFile;
				if (WriteRec)
				{
					WritingFile = string((char*)WriteRec);
				}
				else
				{
					WritingFile = "";
				}

				string InputInstruction;
				string InputFile;
				InputSS >> InputInstruction;
				InputSS >> InputFile;
				if (InputFile == WritingFile && InputInstruction == "write")
				{
					clearrectangle(90, 15, 200, 30);
					outtextxy(90, 15, ReplyInfoRecBuffer);
					outtextxy(16, 41, "当前文件正在被其他用户写入，请稍后再试！");
					char NULLChar[] = "";//清空输入
					strcpy_s((char*)InstructionSend, strlen(NULLChar) + 1, NULLChar);
					goto begin2;
				}

				while (1)
				{
					HANDLE ReplyInfoSpace = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, "ReplyInfoSpace");//不断地从回复信息空间中获取数据
					LPVOID ReplyInfoRec = MapViewOfFile(ReplyInfoSpace, FILE_MAP_ALL_ACCESS, 0, 0, 0);



					if (!ReplyInfoRec)continue;
					if (strlen((char*)ReplyInfoRec))
					{
						MemoryInfoCpy(ReplyInfoRecBuffer, ReplyInfoRec);
						break;//如果获取到的数据，break
					}
				}

				clearrectangle(17, 42, 533, 268);//清空输入文本框

				/*输出返回的信息到信息框*/
				if (ReplyInfoRecBuffer[0] == '/')
				{
					clearrectangle(90, 15, 200, 30);
					outtextxy(90, 15, ReplyInfoRecBuffer);
					outtextxy(16, 41, "跳转成功！");
					continue;
				}
				int CountChangeLine = 1;
				string InputStr(ReplyInfoRecBuffer);
				for (int i = 0; i < InputStr.length(); i++)
				{
					if (InputStr[i] == '\n')
					{
						InputStr[i] = ' ';
						CountChangeLine++;
					}
				}
				istringstream InputSStr(InputStr);
				for (int i = 0; i < CountChangeLine; i++)
				{
					InputSStr >> InputStr;
					outtextxy(16, 41 + 18 * i, InputStr.c_str());
				}
			}
		}
		else if (m.x >= 465 && m.y >= 15 && m.x <= 535 && m.y <= 35) {//返回
			if (m.message == WM_LBUTTONUP) {//左键弹起时
				return;
			}
		}
	}

	/*解除指令空间文件映射*/
	UnmapViewOfFile(InstructionSend);
	CloseHandle(InstructionSpace);

	return;
}


void ShareMemoryClear(LPVOID& ShareMemoryPointer)
{
	char NULLChar[] = "";
	strcpy_s((char*)ShareMemoryPointer, strlen(NULLChar) + 1, NULLChar);
}

void MemoryInfoCpy(char* Buffer, LPVOID& ShareMemoryPointer)
{
	strcpy_s(Buffer, strlen((char*)ShareMemoryPointer) + 1, (char*)ShareMemoryPointer);
	ShareMemoryClear(ShareMemoryPointer);
}