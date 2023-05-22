#include <iostream>
#include <cmath>
#include <cstring>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <io.h>
#include <vector>

#define BUF_SIZE 4096

using namespace std;




/*-------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------基础类-----------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------*/




class INode
{
public:
    char FileName[8];//文件名称
    int type;//表示文件类型，0位目录，1位文件
    int AdAccess[3];//管理员文件保护类型，分别表示只读/可写/可执行
    int UserAccess[3];//普通用户文件保护类型，分别表示只读/可写/可执行
    char ParNode[32];//存放父结点
    char ChildNode[3][32];//存放子结点，文件没有子结点
    char DiskAddress[32];//存放文件对应的内容，目录内容默认为空
    INode()
    {
        for (int i = 0; i < 8; i++)
        {
            FileName[i] = 0;
        }
        type = 0;
        for (int i = 0; i < 3; i++)
        {
            AdAccess[i] = 0;
            UserAccess[i] = 0;
        }
        for (int i = 0; i < 32; i++)
        {
            ParNode[i] = 0;
            DiskAddress[i] = 0;
        }
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                ChildNode[i][j] = 0;
            }
        }
    }
    INode(char* FN, int t, int* AdAcc, int* UserAcc, char* PNode, char** CNode, char* DAdd)
    {
        for (int i = 0; i < 8; i++)
        {
            FileName[i] = FN[i];
        }
        type = t;
        for (int i = 0; i < 3; i++)
        {
            AdAccess[i] = AdAcc[i];
            UserAccess[i] = UserAcc[i];
        }
        for (int i = 0; i < 32; i++)
        {
            ParNode[i] = PNode[i];
            DiskAddress[i] = DAdd[i];
        }
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                ChildNode[i][j] = CNode[i][j];
            }
        }
    }
};





/*-------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------其他函数声明---------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------*/




/*==============================基本函数==============================*/
/*==========翻译函数==========*/
/*8位int数组<==>char字符*/
int* Char2EightInt(char Char);//将符号转换为8位二进制整数数组，用于输入文本
char EightInt2Char(int* EightInt);//将8位二进制整数数组转换为符号，用于输出文本

/*指针==>磁盘块序号*/
int PointerSqe2BlockSqe(int PointerSqe);//将指针序号转换为磁盘块序号

/*32位数组int地址<==>磁盘块地址*/
int* BlockSqe2Add(int BlockSqe);//将磁盘块序号转换为32位地址
int Add2BlockSqe(int* Add);//将32位地址转换为磁盘块序号


/*==========功能函数==========*/
void Init(fstream& Disk_Pointer);//初始化函数，用于将硬盘进行初始化

int GetBlockSqe(fstream& Disk_Pointer);//获取对应指针的当前磁盘块序号
int MatchDocName(fstream& Disk_Pointer, int BlockSqe, string DocName);//将对应的文件名与特定磁盘块中的所有子结点的文件名进行比较，如果匹配，返回磁盘块序号；如果不匹配，返回0
void JumpPointer(fstream& Disk_Pointer, int BlockSqe, int BlockIn);//将指针跳转到指定位置。其中BlockSqe为磁盘块序号，BlockIn为块内地址
void Reply(string ReplyInfo, LPVOID& ShareMemoryPointer);//用于拷贝回复的信息
void ShareMemoryClear(LPVOID& ShareMemoryPointer);//清空共享内存
void MemoryInfoCpy(char* Buffer, LPVOID& ShareMemoryPointer);//将共享内存中的内容拷贝出来
void BitMapChange(fstream& Disk_Pointer, int BlockSqe, int Tag);//将位图区域的某一个值置1或者置0

/*输入函数*/
void InputIntArr(fstream& Disk_Pointer, int* IntArr,int Length);//直接将二进制数输入到硬盘中
void InputChar(fstream& Disk_Pointer, char Char);//将符号转换为二进制数并输入到硬盘中
void InputCharArr(fstream& Disk_Pointer, char* CharArr);//将符号串转换为二进制数并输入到硬盘中
void InputDocName(fstream& Disk_Pointer, int BlockSqe, string DocName);//将文件名输入到硬盘中，输入完后，指针回到原始磁盘块的首位。
void InputParNode(fstream& Disk_Pointer, int BlockSqe, int* IntArr);//将父指针输入到硬盘中，输入完后，指针回到原始磁盘块的首位。
void InputAcc(fstream& Disk_Pointer, int BlockSqe, string Acc);//将保护类型输入到硬盘中
void InputFileContext(fstream& Disk_Pointer, int BlockSqe, int NewDocBlockSqe, string Context);//在对应的磁盘块中输入文件内容磁盘块对应的地址,并在对应的文件磁盘块中输入文件内容
void InputFileType(fstream& Disk_Pointer, int BlockSqe, int FileType);//将文件类型输入到磁盘中

/*输出函数*/
int* Output32Bit(fstream& Disk_Pointer);//输出32位二进制数
int* Output8Bit(fstream& Disk_Pointer);//输出8位二进制数

/*查找函数*/
int FindFreeAreaInINode(fstream& Disk_Pointer);//用于寻找位图中对应的i结点的区域的空闲区域，返回i结点的磁盘块序号，查找完后，指针回到原始磁盘块的首位。
int FindFreeAreaInDoc(fstream& Disk_Pointer);//用于寻找位图中对应的文件区域的空闲区域，返回文件块对应的的磁盘块序号，查找完后，指针回到原始磁盘块的首位
int FindFreeChildNode(fstream& Disk_Pointer, int BlockSqe);//判断特定的磁盘块中是否有空闲的子结点，如果有，返回子结点序号（1/2/3）；如果没有，返回0并定位到磁盘块开头
char* FindDocName(fstream& Disk_Pointer, int BlockSqe);//寻找某一32位地址对应的文件磁盘块对应的文件名，返回文件名数组，查找完后，指针回到原始磁盘块的首位
int FindDocType(fstream& Disk_Pointer, int BlockSqe);//寻找某个磁盘块对应的文件类型
int* FindDocSysAcc(fstream& Disk_Pointer, int BlockSqe);//寻找某个磁盘块对应的系统保护类型
int* FindDocComAcc(fstream& Disk_Pointer, int BlockSqe);//寻找某个磁盘块对应的普通用户保护类型
int* FindDocParNodeAdd(fstream& Disk_Pointer, int BlockSqe);//寻找某个磁盘块对应的父结点地址
int** FindDocChildNodeAdd(fstream& Disk_Pointer, int BlockSqe);//寻找某个磁盘块对应的子结点地址
int* FindDocContextNode(fstream& Disk_Pointer, int BlockSqe);//寻找某个磁盘块对应的内容的地址

/*==============================专用函数==============================*/
/*info*/
int GetINodeRemain(fstream& Disk_Pointer);//获取i结点的剩余空间
int GetDocRemain(fstream& Disk_Pointer);//获取文件区的剩余空间

/*cd*/
string cdAddLocation(fstream& Disk_Pointer, string AddBuf);//将指针跳转至对应路径的目录，并返回路径；如果路径不正确，返回错误信息

/*dir*/
void ShowAllChildNodeName(fstream& Disk_Pointer, int BlockSqe, string& ReplyInfo);//用于寻找目标磁盘块的所有子结点的名字，并将相应的信息录入到ReplyInfo中
void ShowAllInfo(fstream& Disk_Pointer, int BlockSqe, string& ReplyInfo);//将对应磁盘块的所有信息返回到ReplyInfo中
int dirAddLocation(fstream& Disk_Pointer, string AddBuf);//将指针跳转至对应路径的目录；如果路径正确，返回对应文件的磁盘块序号；如果路径不正确，返回0

/*md*/
string mdAddLocation(fstream& Disk_Pointer, string AddBuf);//将指针跳转至对应路径的目录，并返回需要创建的文件名；如果路径不正确，返回一个空串

/*rd*/
int IsDirEmpty(fstream& Disk_Pointer, int BlockSqe);//判断一个目录磁盘块是否为空
int IsFileEmpty(fstream& Disk_Pointer, int BlockSqe);//盘块一个文件磁盘块是否为空
int rdAddLocation(fstream& Disk_Pointer, string AddBuf);//将指针跳转至对应路径的目录，并返回路径；如果路径不正确，返回错误信息
void rdDir(fstream& Disk_Pointer, int BlockSqe);//删除一个目录，如果有子目录，一起删了

/*newfile*/
string FileNameTest(string FileName);//判断一个文件名的格式是否正确
string FileAccTest(string Acc);//判断保护类型的格式是否正确
string newfileAddLocation(fstream& Disk_Pointer, string AddBuf);//将指针跳转至对应路径的目录，并返回需要创建的文件名；如果路径不正确，返回一个空串

/*cat*/
int catAddLocation(fstream& Disk_Pointer, string AddBuf);//将指针跳转至对应路径的目录，并返回路径；如果路径不正确，返回错误信息
string GetFileContext(fstream& Disk_Pointer, int BlockSqe);//获取对应磁盘块的内容

/*del*/
void delDoc(fstream& Disk_Pointer, int BlockSqe);//删除一个文件（指定磁盘块）。

/*copy*/
string GetWindowsFileName(string path);//获取windows下一个文件的文件名
string CopyNewFile(fstream& Disk_Pointer, istringstream& Buf);//在Copy指令下新建一个文件

/*check*/
void ConsistencyCheck(fstream& Disk_Pointer, int BlockSqe);//一致性检查



/*-------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------主函数-----------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------*/







int main()
{
    fstream Disk_Pointer("C:\\Users\\hikari\\Desktop\\课程\\操作系统大作业\\大作业\\HardDisk.txt", ios::in | ios::out);
    Init(Disk_Pointer);
    JumpPointer(Disk_Pointer, 101, 0);

    /*回复信息空间，用于服务器向客户端回复信息*/
    HANDLE ReplyInfoSpace = CreateFileMapping(
        INVALID_HANDLE_VALUE,   // 物理文件句柄
        NULL,   // 默认安全级别
        PAGE_READWRITE,   // 可读可写
        0,   // 高位文件大小
        BUF_SIZE,   // 地位文件大小
        "ReplyInfoSpace"   // 共享内存名称
    );

    LPVOID ReplyInfoSend = MapViewOfFile(
        ReplyInfoSpace,            // 共享内存的句柄
        FILE_MAP_ALL_ACCESS, // 可读写许可
        0,
        0,
        BUF_SIZE
    );

    /*写占用空间，用于高速客户机当前写的状态*/
    HANDLE WriteSpace = CreateFileMapping(
        INVALID_HANDLE_VALUE,   // 物理文件句柄
        NULL,   // 默认安全级别
        PAGE_READWRITE,   // 可读可写
        0,   // 高位文件大小
        BUF_SIZE,   // 地位文件大小
        "WriteSpace"   // 共享内存名称
    );

    //映射缓存区视图，得到指向共享内存的指针
    LPVOID WriteSend = MapViewOfFile(
        WriteSpace,            // 共享内存的句柄
        FILE_MAP_ALL_ACCESS, // 可读写许可
        0,
        0,
        BUF_SIZE
    );
    char WriteSendBuf[BUF_SIZE] = { 0 };

    /*与rd相关的内容*/
    int rdIsNotEmpty = 0;//01标记变量，表示rd的目录是不是空的
    int rdOriSqe = 0;
    int rdTarSqe = 0;
    string rdBuf="";//用户存储输入内容中的地址部分

    int WriteLock = 0;


    /*开始不断地接受信息*/
    while (1)
    {
        //Sleep(1000);
        // 打开共享的文件对象
        HANDLE InstructionSpace = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, "InstructionSpace");
        LPVOID InstructionRec = MapViewOfFile(InstructionSpace, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (!InstructionRec)//判断指针是否为空（共享内存未创建的时候为空）
        {
            printf("Waiting......\n");
            Sleep(1000);
            continue;
        }

        if (strlen((char*)InstructionRec))//判断共享内存中是否有内容
        {
            char UserAccBuf[BUF_SIZE] = { 0 };//用户权限缓存
            HANDLE AccSpace = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, "AccSpace");
            LPVOID AccRec = MapViewOfFile(AccSpace, FILE_MAP_ALL_ACCESS, 0, 0, 0);
            MemoryInfoCpy(UserAccBuf, AccRec);

            char InstructionRecBuffer[BUF_SIZE] = { 0 };
            MemoryInfoCpy(InstructionRecBuffer, InstructionRec);// 将共享内存数据拷贝出来
            istringstream Buf(InstructionRecBuffer);//将内容转换为输入流
            string InstructionBuf;//用于存储输入内容中的指令部分

            Buf >> InstructionBuf;//输入用户输入的指令
            cout << "Get info:" << InstructionRecBuffer << endl;//输出客户端输入的信息
            if (rdIsNotEmpty == 1)
            {
                if (InstructionBuf == "y")
                {
                    rdDir(Disk_Pointer, rdTarSqe);
                    BitMapChange(Disk_Pointer, rdTarSqe, 0);
                    Reply("删除完成", ReplyInfoSend);
                    JumpPointer(Disk_Pointer, rdOriSqe, 0);
                    rdIsNotEmpty = 0;
                    rdOriSqe = 0;
                    rdBuf = "";
                    rdTarSqe = 0;
                    continue;
                }
                else if (InstructionBuf == "n")
                {
                    Reply("操作取消", ReplyInfoSend);
                    JumpPointer(Disk_Pointer, rdOriSqe, 0);
                    rdIsNotEmpty = 0;
                    rdOriSqe = 0;
                    rdBuf = "";
                    rdTarSqe = 0;
                    continue;
                }
                else
                {
                    Reply("指令错误",ReplyInfoSend);
                    JumpPointer(Disk_Pointer, rdOriSqe, 0);
                    rdIsNotEmpty = 0;
                    rdOriSqe = 0;
                    rdBuf = "";
                    rdTarSqe = 0;
                    continue;
                }
            }
            else if (InstructionBuf == "info")
            {
                int OriSqe = GetBlockSqe(Disk_Pointer);
                string ReplyInfo = "";
                string DocSysHost;
                string DocSysSize;
                string DiskBlockSize;
                string SuperBlockSqe;
                string BitMapSqe;
                string RootSqe;
                string INodeSqe;
                string DocSqe;
                DocSysHost = FindDocName(Disk_Pointer, 0);
                JumpPointer(Disk_Pointer, 0, 64);
                DocSysSize = to_string(Add2BlockSqe(Output32Bit(Disk_Pointer)));
                JumpPointer(Disk_Pointer, 0, 96);
                DiskBlockSize = to_string(Add2BlockSqe(Output32Bit(Disk_Pointer)));
                JumpPointer(Disk_Pointer, 0, 128);
                SuperBlockSqe = to_string(Add2BlockSqe(Output32Bit(Disk_Pointer)));
                JumpPointer(Disk_Pointer, 0, 160);
                BitMapSqe = to_string(Add2BlockSqe(Output32Bit(Disk_Pointer)));
                JumpPointer(Disk_Pointer, 0, 192);
                RootSqe = to_string(Add2BlockSqe(Output32Bit(Disk_Pointer)));
                JumpPointer(Disk_Pointer, 0, 224);
                INodeSqe = to_string(Add2BlockSqe(Output32Bit(Disk_Pointer)));
                JumpPointer(Disk_Pointer, 0, 256);
                DocSqe = to_string(Add2BlockSqe(Output32Bit(Disk_Pointer)));
                ReplyInfo = ReplyInfo+"文件系统归属:"+DocSysHost + "\n";
                ReplyInfo = ReplyInfo + "文件系统大小:" + DocSysSize +"Mb" + "\n";
                ReplyInfo = ReplyInfo + "磁盘块大小:" + DiskBlockSize +"b" +"\n";
                ReplyInfo = ReplyInfo + "超级块序号:" + SuperBlockSqe + "\n";
                ReplyInfo = ReplyInfo + "位图序号:" + BitMapSqe + "\n";
                ReplyInfo = ReplyInfo + "根结点序号:" + RootSqe + "\n";
                ReplyInfo = ReplyInfo + "I结点区序号:" + INodeSqe + "\n";
                ReplyInfo = ReplyInfo + "I结点区剩余空间:" + to_string(GetINodeRemain(Disk_Pointer)) + "\n";
                ReplyInfo = ReplyInfo + "文件区序号:" + DocSqe + "\n";
                ReplyInfo = ReplyInfo + "文件区剩余空间:" + to_string(GetDocRemain(Disk_Pointer));
                JumpPointer(Disk_Pointer, OriSqe, 0);
                Reply(ReplyInfo, ReplyInfoSend);
                continue;
            }
            else if (InstructionBuf == "cd")
            {
                string cdBuf;//用户存储输入内容中的地址部分
                Buf >> cdBuf;
                string ReplyInfo= cdAddLocation(Disk_Pointer, cdBuf);
                Reply(ReplyInfo, ReplyInfoSend);
            }
            else if (InstructionBuf == "dir")
            {
                int OriSqe = GetBlockSqe(Disk_Pointer);
                string ReplyInfo="";
                string dirBuf;//用户存储输入内容中的地址部分
                Buf >> dirBuf;
                if (dirBuf == "/s")//如果带/s参数
                {
                    ReplyInfo += "当前目录下的所有文件和子目录为：";
                    ShowAllChildNodeName(Disk_Pointer, GetBlockSqe(Disk_Pointer), ReplyInfo);
                    Reply(ReplyInfo, ReplyInfoSend);
                    JumpPointer(Disk_Pointer, OriSqe, 0);
                    continue;
                }
                int CurBlockSqe=dirAddLocation(Disk_Pointer, dirBuf);//文件指针定位
                if (CurBlockSqe == 0)//如果返回错误信息
                {
                    Reply("地址有误，创建失败！", ReplyInfoSend);
                    continue;
                }
                ReplyInfo += "当前目录下的信息为：";
                ShowAllInfo(Disk_Pointer, CurBlockSqe, ReplyInfo);
                Reply(ReplyInfo, ReplyInfoSend);
                continue;

                JumpPointer(Disk_Pointer, OriSqe, 0);
            }
            else if (InstructionBuf == "md")//创建新目录
            {
                int OriSqe = GetBlockSqe(Disk_Pointer);
                string mdBuf;//用户存储输入内容中的地址部分
                Buf >> mdBuf;
                
                string CurDirName=mdAddLocation(Disk_Pointer, mdBuf);
                if (CurDirName == "")
                {
                    Reply("地址有误，创建失败！", ReplyInfoSend);
                    continue;
                }

                /*判断新建目录的长度*/
                if (CurDirName.length() > 8)//目录名不能大于8位
                {
                    Reply("目录名过长，创建失败！", ReplyInfoSend);
                    continue;
                }

                int NewINodeBlockSqe = FindFreeAreaInINode(Disk_Pointer);

                /*判断文件系统是否还有空间*/
                if (!NewINodeBlockSqe)//如果文件系统汇总没有空间，创建失败
                {
                    Reply("文件系统已无空闲空间，创建失败！", ReplyInfoSend);
                    continue;
                }

                /*判断所创建文件名是否与子结点中文件名重复*/
                if (MatchDocName(Disk_Pointer, GetBlockSqe(Disk_Pointer), CurDirName))
                {
                    Reply("文件名重复，创建失败！", ReplyInfoSend);
                    continue;
                }

                /*判断子结点是不是满了*/
                int FreeChildSqe = FindFreeChildNode(Disk_Pointer, GetBlockSqe(Disk_Pointer));
                if (FreeChildSqe)//没满，将子结点标记为已经使用，并输入32位地址
                {
                    JumpPointer(Disk_Pointer, GetBlockSqe(Disk_Pointer), 112 + 32 * (FreeChildSqe - 1));//跳转到对应的子结点
                    int* INode = BlockSqe2Add(NewINodeBlockSqe);
                    INode[0] = 1;//将首位置为1，表示该子结点已经被使用
                    InputIntArr(Disk_Pointer, INode,32);//输入32位子结点
                    JumpPointer(Disk_Pointer, GetBlockSqe(Disk_Pointer), 0);//跳回开头
                }
                else
                {
                    Reply("当前文件的子文件已达上限，创建失败！", ReplyInfoSend);
                    continue;
                }

                /*对新文件进行初始化*/
                InputDocName(Disk_Pointer, NewINodeBlockSqe, CurDirName);
                InputParNode(Disk_Pointer, NewINodeBlockSqe, BlockSqe2Add(GetBlockSqe(Disk_Pointer)));
                BitMapChange(Disk_Pointer, NewINodeBlockSqe, 1);
                Reply("目录创建成功！", ReplyInfoSend);
                JumpPointer(Disk_Pointer, OriSqe, 0);
                continue;
            }
            else if (InstructionBuf == "rd")
            {
                rdOriSqe = GetBlockSqe(Disk_Pointer);
                Buf >> rdBuf;
                rdTarSqe=rdAddLocation(Disk_Pointer, rdBuf);//寻找应该删除的目标磁盘块
                if (rdTarSqe == 1)//如果是根目录
                {
                    Reply("根目录不可删除，删除失败！", ReplyInfoSend);
                    JumpPointer(Disk_Pointer, rdOriSqe, 0);
                    continue;
                }
                if (FindDocType(Disk_Pointer, rdTarSqe) != 0)//如果目标文件不是目录
                {
                    Reply("目标文件为普通文件，删除失败！", ReplyInfoSend);
                    JumpPointer(Disk_Pointer, rdOriSqe, 0);
                    continue;
                }
                if (rdTarSqe)
                {
                    if (!IsDirEmpty(Disk_Pointer, rdTarSqe))//如果非空
                    {
                        rdIsNotEmpty = 1;
                        Reply("目标目录非空，是否需要删除？y/n", ReplyInfoSend);
                        continue;
                    }
                    else//如果是空的
                    {
                        rdDir(Disk_Pointer, rdTarSqe);//删除对应结点
                        BitMapChange(Disk_Pointer, rdTarSqe, 0);
                        Reply("目标目录删除完成", ReplyInfoSend);
                        JumpPointer(Disk_Pointer, rdOriSqe, 0);
                        rdIsNotEmpty = 0;//将所有系数置0
                        rdOriSqe = 0;
                        rdBuf = "";
                        rdTarSqe = 0;
                        continue;
                    }
                }
                else//如果没有找都目标磁盘块，返回错误信息
                {
                    Reply("地址错误！",ReplyInfoSend);
                    JumpPointer(Disk_Pointer, rdOriSqe, 0);
                    continue;
                }
                JumpPointer(Disk_Pointer, rdOriSqe, 0);
            }
            else if (InstructionBuf == "newfile")
            {
                int OriSqe = GetBlockSqe(Disk_Pointer);
                string newfileNameBuf;//用户存储输入内容中的地址部分
                string newfileAccBuf;
                string newfileContextBuf;
                Buf >> newfileNameBuf;
                Buf >> newfileAccBuf;
                Buf >> newfileContextBuf;

                string CurDirName = newfileAddLocation(Disk_Pointer, newfileNameBuf);
                /*文件名格式检测*/
                string ReplyInfo = FileNameTest(CurDirName);
                if (ReplyInfo != "")
                {
                    Reply(ReplyInfo, ReplyInfoSend);
                    continue;
                }

                /*保护类型格式检测*/
                ReplyInfo = FileAccTest(newfileAccBuf);
                if (ReplyInfo != "")
                {
                    Reply(ReplyInfo, ReplyInfoSend);
                    continue;
                }

                int NewINodeBlockSqe = FindFreeAreaInINode(Disk_Pointer);
                int NewDocBlockSqe = FindFreeAreaInDoc(Disk_Pointer);

                /*判断文件系统是否还有空间*/
                if (!NewINodeBlockSqe)//如果文件系统汇总没有空间，创建失败
                {
                    Reply("文件系统已无空闲空间，创建失败！", ReplyInfoSend);
                    continue;
                }
                if (!NewDocBlockSqe)//如果文件系统汇总没有空间，创建失败
                {
                    Reply("文件系统已无空闲空间，创建失败！", ReplyInfoSend);
                    continue;
                }

                /*判断所创建文件名是否与子结点中文件名重复*/
                if (MatchDocName(Disk_Pointer, GetBlockSqe(Disk_Pointer), CurDirName))
                {
                    Reply("文件名重复，创建失败！", ReplyInfoSend);
                    continue;
                }

                /*判断子结点是不是满了*/
                int FreeChildSqe = FindFreeChildNode(Disk_Pointer, GetBlockSqe(Disk_Pointer));
                if (FreeChildSqe)//没满，将子结点标记为已经使用，并输入32位地址
                {
                    JumpPointer(Disk_Pointer, GetBlockSqe(Disk_Pointer), 112 + 32 * (FreeChildSqe - 1));//跳转到对应的子结点
                    int* INode = BlockSqe2Add(NewINodeBlockSqe);
                    INode[0] = 1;//将首位置为1，表示该子结点已经被使用
                    InputIntArr(Disk_Pointer, INode, 32);//输入32位子结点
                    JumpPointer(Disk_Pointer, GetBlockSqe(Disk_Pointer), 0);//跳回开头
                }
                else
                {
                    Reply("当前文件的子文件已达上限，创建失败！", ReplyInfoSend);
                    continue;
                }

                /*对新文件进行初始化*/
                InputDocName(Disk_Pointer, NewINodeBlockSqe, CurDirName);//输入文件名
                InputParNode(Disk_Pointer, NewINodeBlockSqe, BlockSqe2Add(GetBlockSqe(Disk_Pointer)));//输入父结点地址
                InputAcc(Disk_Pointer, NewINodeBlockSqe, newfileAccBuf);//输入文件保护类型
                InputFileContext(Disk_Pointer, NewINodeBlockSqe, NewDocBlockSqe,newfileContextBuf);//输入文件内容
                InputFileType(Disk_Pointer, NewINodeBlockSqe, 1);//输入文件类型
                BitMapChange(Disk_Pointer, NewINodeBlockSqe, 1);
                BitMapChange(Disk_Pointer, NewDocBlockSqe, 1);
                Reply("文件创建成功！", ReplyInfoSend);
                JumpPointer(Disk_Pointer, OriSqe, 0);
                continue;
            }
            else if (InstructionBuf == "cat")
            {
                int OriSqe = GetBlockSqe(Disk_Pointer);
                string mdBuf;//用户存储输入内容中的地址部分
                Buf >> mdBuf;
                int TarSqe=catAddLocation(Disk_Pointer, mdBuf);
                if (FindDocType(Disk_Pointer, TarSqe) != 1)//如果目标文件不是普通文件
                {
                    Reply("目标文件不是普通文件，指令错误！", ReplyInfoSend);
                    JumpPointer(Disk_Pointer, OriSqe, 0);
                    continue;
                }
                if (TarSqe)
                {
                    int* ComAcc = FindDocComAcc(Disk_Pointer, TarSqe);//查询普通用户访问权限
                    int* SysAcc = FindDocSysAcc(Disk_Pointer, TarSqe);//查询系统用户访问权限
                    if (!((ComAcc[0] == 1 && UserAccBuf[0] == '1') || (SysAcc[0] == 1 && UserAccBuf[0] == '0')))//判断有无权限写入该文件
                    {
                        Reply("无权限读取该文件！", ReplyInfoSend);
                        JumpPointer(Disk_Pointer, OriSqe, 0);
                        continue;
                    }
                    Reply(GetFileContext(Disk_Pointer, Add2BlockSqe(FindDocContextNode(Disk_Pointer, TarSqe))), ReplyInfoSend);
                    JumpPointer(Disk_Pointer, OriSqe, 0);
                    continue;
                }
                else
                {
                    Reply("地址错误！", ReplyInfoSend);
                    JumpPointer(Disk_Pointer, OriSqe, 0);
                    continue;
                }
                JumpPointer(Disk_Pointer, OriSqe, 0);
            }
            else if (InstructionBuf == "write")
            {
                int OriSqe = GetBlockSqe(Disk_Pointer);
                string writeBuf;//用户存储输入内容中的地址部分
                string InputInfo;//待输入的文件内容
                Buf >> writeBuf;
                Buf >> InputInfo;
                int TarSqe=catAddLocation(Disk_Pointer, writeBuf);
                if (FindDocType(Disk_Pointer, TarSqe) != 1)//如果目标文件不是普通文件
                {
                    Reply("目标文件不是普通文件，指令错误！", ReplyInfoSend);
                    JumpPointer(Disk_Pointer, OriSqe, 0);
                    continue;
                }
                if (TarSqe)
                {
                    int* ComAcc = FindDocComAcc(Disk_Pointer, TarSqe);//查询普通用户访问权限
                    int* SysAcc = FindDocSysAcc(Disk_Pointer, TarSqe);//查询系统用户访问权限
                    if (InputInfo.length() > 128)
                    {
                        Reply("文件内容过多，无法写入！", ReplyInfoSend);
                        JumpPointer(Disk_Pointer, OriSqe, 0);
                        continue;
                    }
                    if (!((ComAcc[1] == 1 && UserAccBuf[0] == '1') || (SysAcc[1] == 1 && UserAccBuf[0] == '0')))//判断有无权限写入该文件
                    {
                        Reply("无权限写入该文件！", ReplyInfoSend);
                        JumpPointer(Disk_Pointer, OriSqe, 0);
                        continue;
                    }
                    /*清空源文件*/
                    JumpPointer(Disk_Pointer, Add2BlockSqe(FindDocContextNode(Disk_Pointer, TarSqe)), 0);
                    for (int i = 0; i < 1024; i++)
                    {
                        Disk_Pointer << 0;
                    }
                    JumpPointer(Disk_Pointer, Add2BlockSqe(FindDocContextNode(Disk_Pointer, TarSqe)), 0);
                    
                    /*写时延迟*/
                    strcpy_s((char*)WriteSend, writeBuf.length() + 1, const_cast<char*>(writeBuf.c_str()));
                    Sleep(10000);
                    ShareMemoryClear(WriteSend);

                    /*写入*/
                    InputCharArr(Disk_Pointer, const_cast<char*>(InputInfo.c_str()));
                    Reply("写入成功！", ReplyInfoSend);
                    JumpPointer(Disk_Pointer, OriSqe, 0);
                    continue;
                }
                else
                {
                    Reply("地址错误！", ReplyInfoSend);
                    JumpPointer(Disk_Pointer, OriSqe, 0);
                    continue;
                }
                JumpPointer(Disk_Pointer, OriSqe, 0);
            }
            else if (InstructionBuf == "copy")
            {
                int OriSqe = GetBlockSqe(Disk_Pointer);
                string OriAdd;
                string TarAdd;
                string Parameter;

                Buf >> OriAdd;
                Buf >> TarAdd;
                Buf >> Parameter;

                if (Parameter == "")//如果没有参数
                {
                    /*寻找源文件内容*/
                    int OriAddSqe = catAddLocation(Disk_Pointer, OriAdd);
                    string OriContext;
                    if (OriAddSqe)
                    {
                        OriContext = GetFileContext(Disk_Pointer, Add2BlockSqe(FindDocContextNode(Disk_Pointer, OriAddSqe)));
                    }
                    else
                    {
                        Reply("源地址错误！", ReplyInfoSend);
                        JumpPointer(Disk_Pointer, OriSqe, 0);
                        continue;
                    }

                    int TarAddSqe = catAddLocation(Disk_Pointer, TarAdd);
                    if (TarAddSqe)
                    {
                        int* TarAddDocAdd = FindDocContextNode(Disk_Pointer, TarAddSqe);
                        JumpPointer(Disk_Pointer, Add2BlockSqe(TarAddDocAdd), 0);
                        for (int i = 0; i < 1024; i++)
                        {
                            Disk_Pointer << 0;
                        }
                        JumpPointer(Disk_Pointer, Add2BlockSqe(TarAddDocAdd), 0);
                        InputCharArr(Disk_Pointer, const_cast<char*>(OriContext.c_str()));
                        Reply("复制成功！", ReplyInfoSend);
                        JumpPointer(Disk_Pointer, OriSqe, 0);
                        continue;
                    }
                    else
                    {
                        Reply("目标地址错误！", ReplyInfoSend);
                        JumpPointer(Disk_Pointer, OriSqe, 0);
                        continue;
                    }
                }
                else if(Parameter=="host")
                {
                    string TempOriContext;
                    string OriContext;
                    string OriFileName=GetWindowsFileName(OriAdd);
                    fstream Disk_Pointer2(OriAdd, ios::in || ios::out);

                    /*寻找源文件内容*/
                    if (Disk_Pointer2)
                    {
                        while (!Disk_Pointer2.eof())
                        {
                            Disk_Pointer2 >> TempOriContext;
                            OriContext += TempOriContext;
                        }
                        //GetWindowsFileName(OriAdd,OriFileName)
                    }
                    else
                    {
                        Reply("源地址错误！", ReplyInfoSend);
                        JumpPointer(Disk_Pointer, OriSqe, 0);
                        continue;
                    }
                    Disk_Pointer2.close();

                    /*拷贝到目标文件夹*/
                    string InputInstruciton = "";
                    InputInstruciton += OriFileName;
                    InputInstruciton += " 111111 ";
                    InputInstruciton += OriContext;
                    istringstream Inputsstr(InputInstruciton);
                    Reply(CopyNewFile(Disk_Pointer, Inputsstr), ReplyInfoSend);
                    JumpPointer(Disk_Pointer, OriSqe, 0);
                    continue;
                }
                else
                {
                    Reply("参数错误！", ReplyInfoSend);
                    JumpPointer(Disk_Pointer, OriSqe, 0);
                    continue;
                }

                JumpPointer(Disk_Pointer, OriSqe, 0);
            }
            else if (InstructionBuf == "del")
            {
                int OriSqe = GetBlockSqe(Disk_Pointer);
                string delBuf;//用户存储输入内容中的地址部分
                Buf >> delBuf;
                int TarSqe=catAddLocation(Disk_Pointer, delBuf);
                if (FindDocType(Disk_Pointer, TarSqe) != 1)//如果目标文件不是普通文件
                {
                    Reply("目标文件不是普通文件，指令错误！", ReplyInfoSend);
                    JumpPointer(Disk_Pointer, OriSqe, 0);
                    continue;
                }
                if (TarSqe)
                {
                    delDoc(Disk_Pointer, TarSqe);
                    Reply("删除成功！", ReplyInfoSend);
                    JumpPointer(Disk_Pointer, OriSqe, 0);
                    continue;
                }
                else
                {
                    Reply("地址错误！", ReplyInfoSend);
                    JumpPointer(Disk_Pointer, OriSqe, 0);
                    continue;
                }
            }
            else if (InstructionBuf == "check")
            {
                int OriSqe = GetBlockSqe(Disk_Pointer);
                ConsistencyCheck(Disk_Pointer,101);
                JumpPointer(Disk_Pointer, OriSqe, 0);
                Reply("文件一致性检查完成！", ReplyInfoSend);
                continue;
            }
            else
            {
                Reply("指令错误！", ReplyInfoSend);
                continue;
            }
        }
        else
        {
            printf("Waiting......\n");
            Sleep(1000);
        }
    }
    return 0;

    /*解除回复信息空间文件映射*/
    UnmapViewOfFile(ReplyInfoSend);
    CloseHandle(ReplyInfoSpace);
}




/*-------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------其他函数体-------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------*/
/*==========翻译函数==========*/


int* Char2EightInt(char Char)
{
    int IntNum = int(Char);//将字符转换为ASCII码
    int* BiInt;//输出数组
    BiInt = new int[8];
    for (int i = 0; i < 8; i++)//用除n取余法转换为二进制数
    {
        BiInt[7 - i] = IntNum % 2;
        IntNum = IntNum / 2;
    }
    return BiInt;
}
char EightInt2Char(int* EightInt)
{
    int IntNum = 0;
    for (int i = 7; i >= 0; i--)//将二进制数进行累加
    {
        IntNum += EightInt[i] * pow(2, 7 - i);
    }
    return char(IntNum);//返回ASCII码值
}

int PointerSqe2BlockSqe(int PointerSqe)
{
    return PointerSqe / 1024;
}

int Add2BlockSqe(int* Add)
{
    int BlockSqe = 0;
    for (int i = 21; i >= 5; i--)//将17位二进制数翻译为磁盘块地址
    {
        BlockSqe += Add[i] * pow(2, 21 - i);
    }
    return BlockSqe;
}

int* BlockSqe2Add(int BlockSqe)
{
    int* Add;
    Add = new int[32];
    for (int i = 0; i < 32; i++)
    {
        Add[i] = 0;
    }
    for (int i = 21; i >= 5; i--)//用除n取余法转换为二进制数
    {
        Add[i] = BlockSqe % 2;
        BlockSqe = BlockSqe / 2;
    }
    return Add;
}



/*==========功能函数==========*/



void Init(fstream& Disk_Pointer)
{
    /*初始化根目录*/
    Disk_Pointer.seekg(1024 * 101, ios::beg);
    char RootDirName[] = "cmy\0\0\0\0\0";//根目录名
    InputCharArr(Disk_Pointer, RootDirName);
    int FileType[] = { 0,0,0,0,0,0,0,0 };//表示文件类型为目录
    InputIntArr(Disk_Pointer, FileType,8);
    int Access[] = { 0,0,0,0,0,0,0,0 };//目录不讨论访问权限问题，全部置为0
    InputIntArr(Disk_Pointer, Access,8);
    int ParNode[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 };
    //根目录没有父结点
    InputIntArr(Disk_Pointer, ParNode,32);
    int ChildNode[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 };
    //根目录初始时没有子结点
    InputIntArr(Disk_Pointer, ChildNode,96);
    //目录没有文件内容
    int DiskAddress[] = { 0,0,0,0,0,0,0,0 ,0,0,0,0,0,0,0,0 ,0,0,0,0,0,0,0,0 ,0,0,0,0,0,0,0,0 };
    InputIntArr(Disk_Pointer, DiskAddress,32);

    /*初始化超级块区*/
    JumpPointer(Disk_Pointer, 0, 0);
    InputCharArr(Disk_Pointer, const_cast<char*>("cmy"));
    JumpPointer(Disk_Pointer, 0, 64);
    InputIntArr(Disk_Pointer, BlockSqe2Add(100), 32);
    JumpPointer(Disk_Pointer, 0, 96);
    InputIntArr(Disk_Pointer, BlockSqe2Add(1024), 32);
    JumpPointer(Disk_Pointer, 0, 124);
    InputIntArr(Disk_Pointer, BlockSqe2Add(0), 32);
    JumpPointer(Disk_Pointer, 0, 160);
    InputIntArr(Disk_Pointer, BlockSqe2Add(1), 32);
    JumpPointer(Disk_Pointer, 0, 192);
    InputIntArr(Disk_Pointer, BlockSqe2Add(101), 32);
    JumpPointer(Disk_Pointer, 0, 224);
    InputIntArr(Disk_Pointer, BlockSqe2Add(102), 32);
    JumpPointer(Disk_Pointer, 0, 256);
    InputIntArr(Disk_Pointer, BlockSqe2Add(50102), 32);

    /*初始化位图区*/
    JumpPointer(Disk_Pointer, 1, 0);//跳转到位图区
    for (int i = 0; i < 102; i++)//将前101个磁盘块置为已使用
    {
        Disk_Pointer << 1;
    }
    for (int i = 0; i < 1024 * 100 - 102; i++)//将后面的所有磁盘块置为未使用
    {
        Disk_Pointer << 0;
    }
}

int GetBlockSqe(fstream& Disk_Pointer)
{
    int PointerSqe = Disk_Pointer.tellg();
    return PointerSqe2BlockSqe(PointerSqe);
}

int MatchDocName(fstream& Disk_Pointer, int BlockSqe, string DocName)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    for (int i = 0; i < 3;i++)//循环3次判断子结点文件名
    {
        JumpPointer(Disk_Pointer, BlockSqe, 112+32*i);//跳转到子结点部分
        int* BitDocName = Output32Bit(Disk_Pointer);//读入32位子结点地址
        char* CharDocName = FindDocName(Disk_Pointer, Add2BlockSqe(BitDocName));//寻找子结点文件名
        if (string(CharDocName) == DocName)//判断子结点文件名是不是要找的东西
        {
            JumpPointer(Disk_Pointer, OriSqe, 0);
            return Add2BlockSqe(BitDocName);//如果是，返回子结点对应的磁盘块号
        }
    }
    JumpPointer(Disk_Pointer, OriSqe,0);
    return 0;//如果不是，返回0
}

void JumpPointer(fstream& Disk_Pointer, int BlockSqe, int BlockIn)
{
    Disk_Pointer.seekg(1024 * BlockSqe + BlockIn, ios::beg);
}

void Reply(string ReplyInfo, LPVOID& ShareMemoryPointer)
{
    strcpy_s((char*)ShareMemoryPointer, strlen(ReplyInfo.c_str()) + 1, ReplyInfo.c_str());//发送错误信息
    cout << "Return Info:" << ReplyInfo << endl;
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

void BitMapChange(fstream& Disk_Pointer, int BlockSqe, int Tag)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    JumpPointer(Disk_Pointer, 1, BlockSqe);
    Disk_Pointer << Tag;
    JumpPointer(Disk_Pointer, BlockSqe, 0);
}

/*输入函数*/

void InputCharArr(fstream& Disk_Pointer, char* CharArr)
{
    for (int i = 0; i < strlen(CharArr); i++)
    {
        InputChar(Disk_Pointer, CharArr[i]);
    }
}

void InputChar(fstream& Disk_Pointer, char Char)
{
    int* InputInt;
    InputInt = new int[8];
    InputInt = Char2EightInt(Char);
    InputIntArr(Disk_Pointer, InputInt,8);
}

void InputIntArr(fstream& Disk_Pointer, int* IntArr, int Length)
{
    for (int i = 0; i < Length; i++)
    {
        Disk_Pointer << IntArr[i];
    }
}

void InputDocName(fstream& Disk_Pointer,int BlockSqe,string DocName)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    JumpPointer(Disk_Pointer, BlockSqe, 0);
    int DocNameLength = DocName.length();//获取文件名的长度
    char* CharDocName;
    CharDocName = new char[DocNameLength];//申请一个临时数组存放文件名
    strcpy_s(CharDocName, DocNameLength+1, DocName.c_str());//将文件名汇总内容拷贝到临时数组
    InputCharArr(Disk_Pointer, CharDocName);//输入文件名
    JumpPointer(Disk_Pointer, OriSqe, 0);
}

void InputParNode(fstream& Disk_Pointer, int BlockSqe, int* IntArr)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    JumpPointer(Disk_Pointer, BlockSqe, 80);
    InputIntArr(Disk_Pointer, IntArr,32);
    JumpPointer(Disk_Pointer, OriSqe, 0);
}

void InputAcc(fstream& Disk_Pointer, int BlockSqe, string Acc)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    JumpPointer(Disk_Pointer, BlockSqe, 72);
    int* TempInputIntArr;
    TempInputIntArr = new int[8];
    for (int i = 0; i < 8; i++)
    {
        TempInputIntArr[i] = 0;
    }
    for (int i = 1; i < 4; i++)
    {
        TempInputIntArr[i] = Acc[i - 1] - 48;
    }
    for (int i = 5; i < 8; i++)
    {
        TempInputIntArr[i] = Acc[i - 2] - 48;
    }
    InputIntArr(Disk_Pointer, TempInputIntArr, 8);
    JumpPointer(Disk_Pointer, OriSqe, 0);
}
void InputFileContext(fstream& Disk_Pointer, int BlockSqe, int NewDocBlockSqe, string Context)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    JumpPointer(Disk_Pointer, BlockSqe, 208);
    InputIntArr(Disk_Pointer, BlockSqe2Add(NewDocBlockSqe), 32);
    JumpPointer(Disk_Pointer, NewDocBlockSqe, 0);
    InputCharArr(Disk_Pointer, const_cast<char*>(Context.c_str()));
    JumpPointer(Disk_Pointer, OriSqe, 0);
}
void InputFileType(fstream& Disk_Pointer, int BlockSqe, int FileType)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    JumpPointer(Disk_Pointer, BlockSqe, 71);
    Disk_Pointer << FileType;
    JumpPointer(Disk_Pointer, OriSqe, 0);
}

/*输出函数*/
int* Output32Bit(fstream& Disk_Pointer)
{
    int* Output32Bit;
    Output32Bit = new int[32];
    char TempChar;
    for (int i = 0; i < 32; i++)
    {
        Disk_Pointer >> TempChar;
        Output32Bit[i] = int(TempChar) - 48;
    }
    return Output32Bit;
}
int* Output8Bit(fstream& Disk_Pointer)
{
    int* Output8Bit;
    Output8Bit = new int[8];
    char TempChar;
    for (int i = 0; i < 8; i++)
    {
        Disk_Pointer >> TempChar;
        Output8Bit[i] = int(TempChar) - 48;
    }
    return Output8Bit;
}

/*查找函数*/
int FindFreeAreaInDoc(fstream& Disk_Pointer)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    JumpPointer(Disk_Pointer, 1, 0);//遍历位图区中文件的部分
    int i = 50102;
    char TempChar;
    for (i; i < 100102; i++)
    {
        Disk_Pointer >> TempChar;
        if (TempChar == '0')
        {
            JumpPointer(Disk_Pointer, OriSqe, 0);
            return i;
        }
    }
    JumpPointer(Disk_Pointer, OriSqe, 0);
    return 0;//如果没找到，返回0
    
}

int FindFreeAreaInINode(fstream& Disk_Pointer)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    int i = 102;
    JumpPointer(Disk_Pointer, 1, i);
    char TempChar;
    for (i; i < 50102; i++)//遍历位图区中i结点的部分
    {
        Disk_Pointer >> TempChar;
        if (TempChar == '0')
        {
            JumpPointer(Disk_Pointer, OriSqe, 0);
            return i;
        }
    }
    JumpPointer(Disk_Pointer, OriSqe, 0);
    return 0;//如果没找到，返回0
}

char* FindDocName(fstream& Disk_Pointer, int BlockSqe)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    char* DocName;//应该返回的文件名
    DocName = new char[8];
    int* TempIntArr;//用于装载用于翻译的数字
    JumpPointer(Disk_Pointer, BlockSqe, 0);//跳转到对应位置
    for (int i = 0; i < 8; i++)
    {
        TempIntArr = Output8Bit(Disk_Pointer);
        DocName[i] = EightInt2Char(TempIntArr);//将对应的二进制数组转换为符号
    }
    JumpPointer(Disk_Pointer, OriSqe, 0);
    return DocName;//返回文件名
}

int FindFreeChildNode(fstream& Disk_Pointer, int BlockSqe)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    char IsFree;

    JumpPointer(Disk_Pointer, BlockSqe, 112);//跳转到第一个子结点的位置
    Disk_Pointer >> IsFree;
    if (IsFree == '0')//如果子结点没有被使用
    {
        Disk_Pointer.seekg(-1, ios::cur);//退一位
        JumpPointer(Disk_Pointer, OriSqe, 0);
        return 1;//返回子结点序号
    }

    JumpPointer(Disk_Pointer, BlockSqe, 112 + 32);//跳转到第二个子结点的位置
    Disk_Pointer >> IsFree;
    if (IsFree == '0')//如果子结点没有被使用
    {
        Disk_Pointer.seekg(-1, ios::cur);//退一位
        JumpPointer(Disk_Pointer, OriSqe, 0);
        return 2;//返回子结点序号
    }

    JumpPointer(Disk_Pointer, BlockSqe, 112 + 32 + 32);//跳转到第三个子结点的位置
    Disk_Pointer >> IsFree;
    if (IsFree == '0')//如果子结点没有被使用
    {
        Disk_Pointer.seekg(-1, ios::cur);//退一位
        JumpPointer(Disk_Pointer, OriSqe, 0);
        return 3;//返回子结点序号
    }

    JumpPointer(Disk_Pointer, OriSqe, 0);
    return 0;
}
int FindDocType(fstream& Disk_Pointer, int BlockSqe)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    char TempChar;
    JumpPointer(Disk_Pointer, BlockSqe, 71);
    Disk_Pointer >> TempChar;
    JumpPointer(Disk_Pointer, OriSqe, 0);
    return int(TempChar) - 48;
}
int* FindDocSysAcc(fstream& Disk_Pointer, int BlockSqe)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    char TempChar;
    int* SysAcc;
    SysAcc = new int[3];
    JumpPointer(Disk_Pointer, BlockSqe, 73);
    for (int i = 0; i < 3; i++)
    {
        Disk_Pointer >> TempChar;
        SysAcc[i] = TempChar - 48;
    }
    JumpPointer(Disk_Pointer, OriSqe, 0);
    return SysAcc;
}
int* FindDocComAcc(fstream& Disk_Pointer, int BlockSqe)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    char TempChar;
    int* ComAcc;
    ComAcc = new int[3];
    JumpPointer(Disk_Pointer, BlockSqe, 77);
    for (int i = 0; i < 3; i++)
    {
        Disk_Pointer >> TempChar;
        ComAcc[i] = TempChar - 48;
    }
    JumpPointer(Disk_Pointer, OriSqe, 0);
    return ComAcc;
}
int* FindDocParNodeAdd(fstream& Disk_Pointer, int BlockSqe)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    char TempChar;
    int* ParNodeAdd;
    ParNodeAdd = new int[32];
    JumpPointer(Disk_Pointer, BlockSqe, 80);
    for (int i = 0; i < 32; i++)
    {
        Disk_Pointer >> TempChar;
        ParNodeAdd[i] = TempChar - 48;
    }
    JumpPointer(Disk_Pointer, OriSqe, 0);
    return ParNodeAdd;
}
int** FindDocChildNodeAdd(fstream& Disk_Pointer, int BlockSqe)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    char TempChar;
    int** ChildNodeAdd;
    ChildNodeAdd = new int* [3];
    for (int i = 0; i < 32; i++)
    {
        ChildNodeAdd[i] = new int[32];
    }
    JumpPointer(Disk_Pointer, BlockSqe, 112);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            Disk_Pointer >> TempChar;
            ChildNodeAdd[i][j] = TempChar - 48;
        }
    }
    JumpPointer(Disk_Pointer, OriSqe, 0);
    return ChildNodeAdd;
}
int* FindDocContextNode(fstream& Disk_Pointer, int BlockSqe)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    char TempChar;
    int* ParNodeAdd;
    ParNodeAdd = new int[32];
    JumpPointer(Disk_Pointer, BlockSqe, 208);
    for (int i = 0; i < 32; i++)
    {
        Disk_Pointer >> TempChar;
        ParNodeAdd[i] = TempChar - 48;
    }
    JumpPointer(Disk_Pointer, OriSqe, 0);
    return ParNodeAdd;
}

/*==============================专用函数==============================*/
/*info*/
int GetINodeRemain(fstream& Disk_Pointer)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    int CountRemain = 0;
    JumpPointer(Disk_Pointer, 1, 102);
    char TempChar;
    for (int i = 102; i < 50102; i++)
    {
        Disk_Pointer >> TempChar;
        if (TempChar == '0')
        {
            CountRemain++;
        }
    }
    JumpPointer(Disk_Pointer, OriSqe, 0);
    return CountRemain;
}
int GetDocRemain(fstream& Disk_Pointer)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    int CountRemain = 0;
    JumpPointer(Disk_Pointer, 1, 50102);
    char TempChar;
    for (int i = 50102; i < 102400; i++)
    {
        Disk_Pointer >> TempChar;
        if (TempChar == '0')
        {
            CountRemain++;
        }
    }
    JumpPointer(Disk_Pointer, OriSqe, 0);
    return CountRemain;
}

/*cd*/
string cdAddLocation(fstream& Disk_Pointer, string AddBuf)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    string OriAddBuf=AddBuf;
    string CurDirName;//用于暂时存储当前位置
    if (AddBuf.length() == 0)//如果地址为空，返回错误信息
    {
        return "跳转地址缺失！";
    }

    if (AddBuf[0] == '/')
    {
        JumpPointer(Disk_Pointer, 101, 0);//从根结点开始检索
        /*输入指令的分隔符/进行预处理*/
        for (int i = 0; i < AddBuf.length(); i++)//先处理地址中的/符号
        {
            if (AddBuf[i] == '/')AddBuf[i] = ' ';//将/替换为空格，便于输入
        }

        istringstream AddStr(AddBuf);
        AddStr >> CurDirName;

        /*判断根地址是否有问题*/
        if (CurDirName != "cmy")//如果根地址有误
        {
            JumpPointer(Disk_Pointer, OriSqe, 0);//跳转回原来的位置
            return "地址错误！";
        }
        if (AddStr.eof())
        {
            JumpPointer(Disk_Pointer, 101, 0);
            return "/cmy";
        }
        /*通过子结点不断进行定位*/
        while (1)//当输入字符串AddressBuf中还有内容
        {
            AddStr >> CurDirName;//输入下一个目录
            
            int ChildBlockSqe = MatchDocName(Disk_Pointer, GetBlockSqe(Disk_Pointer), CurDirName);
            if (ChildBlockSqe)//如果找到了对应的子文件
            {
                if (FindDocType(Disk_Pointer, GetBlockSqe(Disk_Pointer)) != 0)
                {
                    return "地址错误！";
                }
                JumpPointer(Disk_Pointer, ChildBlockSqe, 0);//跳转到对应的位置
            }
            else//如果没有知道对应的子文件
            {
                JumpPointer(Disk_Pointer, OriSqe, 0);//跳转回原来的位置
                return "地址错误！";
            }
            if (AddStr.eof())//当输入完一个目录之后到底输入底部，结束
            {
                return OriAddBuf;
            }
        }
    }
    else
    {
        return "地址错误！";
    }
}

/*dir*/
void ShowAllChildNodeName(fstream& Disk_Pointer, int BlockSqe, string& ReplyInfo)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    char* ChildNodeName;
    int* ChildNodeAdd;
    int FileType; 

    JumpPointer(Disk_Pointer, BlockSqe, 112);
    ChildNodeAdd = Output32Bit(Disk_Pointer);
    FileType = FindDocType(Disk_Pointer, Add2BlockSqe(ChildNodeAdd));
    if (ChildNodeAdd[0] == 1)//如果这个子结点被占用了
    {
        int ChildNodeSqe = Add2BlockSqe(ChildNodeAdd);
        ChildNodeName = FindDocName(Disk_Pointer, ChildNodeSqe);
        ReplyInfo = ReplyInfo  + "\n" + string(ChildNodeName);
    }

    JumpPointer(Disk_Pointer, BlockSqe, 112+32);
    ChildNodeAdd = Output32Bit(Disk_Pointer);
    FileType = FindDocType(Disk_Pointer, Add2BlockSqe(ChildNodeAdd));
    if (ChildNodeAdd[0] == 1)//如果这个子结点被占用了
    {
        int ChildNodeSqe = Add2BlockSqe(ChildNodeAdd);
        ChildNodeName = FindDocName(Disk_Pointer, ChildNodeSqe);
        ReplyInfo = ReplyInfo  + "\n" + string(ChildNodeName);
    }

    JumpPointer(Disk_Pointer, BlockSqe, 112+32+32);
    ChildNodeAdd = Output32Bit(Disk_Pointer);
    FileType = FindDocType(Disk_Pointer, Add2BlockSqe(ChildNodeAdd));
    if (ChildNodeAdd[0] == 1)//如果这个子结点被占用了
    {
        int ChildNodeSqe = Add2BlockSqe(ChildNodeAdd);
        ChildNodeName = FindDocName(Disk_Pointer, ChildNodeSqe);
        ReplyInfo = ReplyInfo  + "\n" + string(ChildNodeName);
    }

    JumpPointer(Disk_Pointer, OriSqe, 0);
}

int dirAddLocation(fstream& Disk_Pointer, string AddBuf)
{
    string CurDirName;//用于暂时存储当前位置
    if (AddBuf.length() == 0)//如果地址为空，返回当前磁盘块序号
    {
        return GetBlockSqe(Disk_Pointer);
    }

    if (AddBuf[0] == '/')
    {
        JumpPointer(Disk_Pointer, 101, 0);//从根结点开始检索
        /*输入指令的分隔符/进行预处理*/
        for (int i = 0; i < AddBuf.length(); i++)//先处理地址中的/符号
        {
            if (AddBuf[i] == '/')AddBuf[i] = ' ';//将/替换为空格，便于输入
        }

        istringstream AddStr(AddBuf);
        AddStr >> CurDirName;

        /*判断根地址是否有问题*/
        if (CurDirName != "cmy")//如果根地址有误
        {
            return 0;
        }
        /*通过子结点不断进行定位*/
        while (1)//当输入字符串AddressBuf中还有内容
        {
            AddStr >> CurDirName;//输入下一个目录
            int ChildBlockSqe = MatchDocName(Disk_Pointer, GetBlockSqe(Disk_Pointer), CurDirName);
            if (ChildBlockSqe)//如果找到了对应的子文件
            {
                JumpPointer(Disk_Pointer, ChildBlockSqe, 0);//跳转到对应的位置
            }
            else//如果没有知道对应的子文件
            {
                return 0;
            }
            if (AddStr.eof())//当输入完一个目录之后到底输入底部，结束
            {
                return GetBlockSqe(Disk_Pointer);
            }
        }
    }
    else
    {
        return 0;
    }
}

void ShowAllInfo(fstream& Disk_Pointer, int BlockSqe, string& ReplyInfo)
{
    char TempChar;
    int OriSqe = GetBlockSqe(Disk_Pointer);
    ReplyInfo = ReplyInfo + "\n" +"目录名：" +string(FindDocName(Disk_Pointer, BlockSqe));

    JumpPointer(Disk_Pointer, BlockSqe, 73);//跳转到管理员保护类型位置
    ReplyInfo = ReplyInfo + "\n" + "管理员保护类型：";
    for (int i = 0; i < 3; i++)//读取管理员保护类型
    {
        Disk_Pointer >> TempChar;
        ReplyInfo += TempChar;
    }

    JumpPointer(Disk_Pointer, BlockSqe, 77);//跳转到普通用户保护类型位置
    ReplyInfo = ReplyInfo + "\n" + "普通用户保护类型：";
    for (int i = 0; i < 3; i++)//读取普通用户保护类型
    {
        Disk_Pointer >> TempChar;
        ReplyInfo += TempChar;
    }

    JumpPointer(Disk_Pointer, BlockSqe, 80);//跳转到父结点位置
    ReplyInfo = ReplyInfo + "\n" + "父结点地址：";
    for (int i = 0; i < 32; i++)//读取父结点地址
    {
        Disk_Pointer >> TempChar;
        ReplyInfo += TempChar;
    }

    JumpPointer(Disk_Pointer, BlockSqe, 112);//跳转到子结点位置
    ReplyInfo = ReplyInfo + "\n" + "子结点地址：";
    for (int i = 0; i < 3; i++)//读取子结点地址
    {
        ReplyInfo += "\n";
        for (int j = 0; j < 32; j++)
        {
            Disk_Pointer >> TempChar;
            ReplyInfo += TempChar;
        }
    }

    JumpPointer(Disk_Pointer, OriSqe, 0);
}

/*md*/
string mdAddLocation(fstream& Disk_Pointer, string AddBuf)
{
    string ErrorReturn = "";
    string CurDirName;//用于暂时存储当前位置
    if (AddBuf.length() == 0)//如果地址为空，错误
    {
        return ErrorReturn;
    }

    /*判断所给地址是否为绝对地址，如果是，跳转到相应位置并返回需要创建的文件名，如果不是，直接返回需要创建的文件名*/
    if (AddBuf[0] == '/')
    {
        JumpPointer(Disk_Pointer, 101, 0);//从根结点开始检索
        /*输入指令的分隔符/进行预处理*/
        for (int i = 0; i < AddBuf.length(); i++)//先处理地址中的/符号
        {
            if (AddBuf[i] == '/')AddBuf[i] = ' ';//将/替换为空格，便于输入
        }

        istringstream AddStr(AddBuf);
        AddStr >> CurDirName;

        /*判断根地址是否有问题*/
        if (CurDirName != "cmy")//如果根地址有误
        {
            return ErrorReturn;
        }

        /*通过子结点不断进行定位*/
        while (1)//当输入字符串AddressBuf中还有内容
        {
            AddStr >> CurDirName;//输入下一个目录
            if (AddStr.eof())//当输入完一个目录之后到底输入底部，那么这时候这个名字就是对应的文件名
            {
                break;
            }
            int ChildBlockSqe = MatchDocName(Disk_Pointer, GetBlockSqe(Disk_Pointer), CurDirName);
            if (ChildBlockSqe)//如果找到了对应的子文件
            {
                if (FindDocType(Disk_Pointer, ChildBlockSqe) != 0)//如果子文件不是目录
                {
                    return ErrorReturn;
                }
                JumpPointer(Disk_Pointer, ChildBlockSqe, 0);//跳转到对应的位置
            }
            else//如果没有知道对应的子文件
            {
                return ErrorReturn;
            }
        }
    }
    else
    {
        for (int i = 1; i < AddBuf.length(); i++)//判断文件名中有没有/符号
        {
            if (AddBuf[i] == '/')
            {
                return ErrorReturn;
            }
        }
        CurDirName = AddBuf;
    }
    for (int i = 0; i < CurDirName.length(); i++)
    {
        if (CurDirName[i] == '.')
        {
            return ErrorReturn;
        }
    }
    return CurDirName;
}

/*rd*/
int IsDirEmpty(fstream& Disk_Pointer, int BlockSqe)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    char TempChar;
    for (int i = 0; i < 3; i++)
    {
        JumpPointer(Disk_Pointer, BlockSqe, 112+32*i);
        Disk_Pointer >> TempChar;
        if (TempChar == '1')
        {
            JumpPointer(Disk_Pointer, OriSqe, 0);
            return 0;
        }
    }
    return 1;
    JumpPointer(Disk_Pointer, OriSqe,0);
}

int IsFileEmpty(fstream& Disk_Pointer, int BlockSqe)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    JumpPointer(Disk_Pointer, BlockSqe, 0);
    char TempChar;
    for (int i = 0; i < 1024; i++)
    {
        Disk_Pointer >> TempChar;
        if (TempChar == '1')return 1;
    }
    JumpPointer(Disk_Pointer, OriSqe, 0);
    return 0;
}

int rdAddLocation(fstream& Disk_Pointer, string AddBuf)
{
    string CurDirName;//用于暂时存储当前位置
    if (AddBuf.length() == 0)//如果地址为空，返回当前磁盘块序号
    {
        return GetBlockSqe(Disk_Pointer);
    }

    if (AddBuf[0] == '/')
    {
        JumpPointer(Disk_Pointer, 101, 0);//从根结点开始检索
        /*输入指令的分隔符/进行预处理*/
        for (int i = 0; i < AddBuf.length(); i++)//先处理地址中的/符号
        {
            if (AddBuf[i] == '/')AddBuf[i] = ' ';//将/替换为空格，便于输入
        }

        istringstream AddStr(AddBuf);
        AddStr >> CurDirName;

        /*判断根地址是否有问题*/
        if (CurDirName != "cmy")//如果根地址有误
        {
            return 0;
        }
        /*通过子结点不断进行定位*/
        while (1)//当输入字符串AddressBuf中还有内容
        {
            AddStr >> CurDirName;//输入下一个目录
            int ChildBlockSqe = MatchDocName(Disk_Pointer, GetBlockSqe(Disk_Pointer), CurDirName);
            if (ChildBlockSqe)//如果找到了对应的子文件
            {
                JumpPointer(Disk_Pointer, ChildBlockSqe, 0);//跳转到对应的位置
            }
            else//如果没有找到对应的子文件
            {
                return 0;
            }
            if (AddStr.eof())//当输入完一个目录之后到底输入底部，结束
            {
                if (FindDocType(Disk_Pointer, GetBlockSqe(Disk_Pointer)) != 0)
                {
                    return 0;
                }
                return GetBlockSqe(Disk_Pointer);
            }
        }
    }
    else
    {
        return 0;
    }
}

void rdDir(fstream& Disk_Pointer, int BlockSqe)
{
    int FileType = FindDocType(Disk_Pointer, BlockSqe);
    if (FileType == 0)//如果对应的是目录
    {
        if (IsDirEmpty(Disk_Pointer, BlockSqe))
        {
            /*将父结点的对应子结点地址清除*/
            int* ParNode = FindDocParNodeAdd(Disk_Pointer, BlockSqe);
            char* DocName = FindDocName(Disk_Pointer, BlockSqe);
            int** ParChildNode = FindDocChildNodeAdd(Disk_Pointer, Add2BlockSqe(ParNode));

            if (string(DocName) == string(FindDocName(Disk_Pointer, Add2BlockSqe(ParChildNode[0]))))
            {
                JumpPointer(Disk_Pointer, Add2BlockSqe(ParNode), 112);
                for (int i = 0; i < 32; i++)
                {
                    Disk_Pointer << 0;
                }
            }
            else if (string(DocName) == string(FindDocName(Disk_Pointer, Add2BlockSqe(ParChildNode[1]))))
            {
                JumpPointer(Disk_Pointer, Add2BlockSqe(ParNode), 112 + 32);
                for (int i = 0; i < 32; i++)
                {
                    Disk_Pointer << 0;
                }
            }
            else if (string(DocName) == string(FindDocName(Disk_Pointer, Add2BlockSqe(ParChildNode[2]))))
            {
                JumpPointer(Disk_Pointer, Add2BlockSqe(ParNode), 112 + 64);
                for (int i = 0; i < 32; i++)
                {
                    Disk_Pointer << 0;
                }
            }

            /*将目录i结点中内容删除*/
            JumpPointer(Disk_Pointer, BlockSqe, 0);
            BitMapChange(Disk_Pointer, GetBlockSqe(Disk_Pointer), 0);//将位图中对应i结点的位置清除
            for (int i = 0; i < 1024    ; i++)
            {
                Disk_Pointer << 0;
            }
            return;
        }
        else
        {
            int** TempIntArr = FindDocChildNodeAdd(Disk_Pointer, BlockSqe);
            /*删子结点*/
            if (TempIntArr[0][0] == 1)
            {
                rdDir(Disk_Pointer, Add2BlockSqe(TempIntArr[0]));
            }
            if (TempIntArr[1][0] == 1)
            {
                rdDir(Disk_Pointer, Add2BlockSqe(TempIntArr[1]));
            }
            if (TempIntArr[2][0] == 1)
            {
                rdDir(Disk_Pointer, Add2BlockSqe(TempIntArr[2]));
            }

            /*删自己*/
            if (IsDirEmpty(Disk_Pointer, BlockSqe))
            {
                rdDir(Disk_Pointer, BlockSqe);
            }
        }
    }
    else//如果对应的是文件
    {
        int* DocContextAdd = FindDocContextNode(Disk_Pointer, BlockSqe);

        /*将文件结点的内容清除*/
        BitMapChange(Disk_Pointer, Add2BlockSqe(DocContextAdd),0);
        JumpPointer(Disk_Pointer, Add2BlockSqe(DocContextAdd), 0);
        for (int i = 0; i < 1024; i++)
        {
            Disk_Pointer << 0;
        }

        /*将父结点的对应子结点地址清除*/
        int* ParNode = FindDocParNodeAdd(Disk_Pointer, BlockSqe);
        char* DocName = FindDocName(Disk_Pointer, BlockSqe);
        int** ParChildNode = FindDocChildNodeAdd(Disk_Pointer, Add2BlockSqe(ParNode));

        if (string(DocName) == string(FindDocName(Disk_Pointer, Add2BlockSqe(ParChildNode[0]))))
        {
            JumpPointer(Disk_Pointer, Add2BlockSqe(ParNode), 112);
            for (int i = 0; i < 32; i++)
            {
                Disk_Pointer << 0;
            }
        }
        else if (string(DocName) == string(FindDocName(Disk_Pointer, Add2BlockSqe(ParChildNode[1]))))
        {
            JumpPointer(Disk_Pointer, Add2BlockSqe(ParNode), 112 + 32);
            for (int i = 0; i < 32; i++)
            {
                Disk_Pointer << 0;
            }
        }
        else if (string(DocName) == string(FindDocName(Disk_Pointer, Add2BlockSqe(ParChildNode[2]))))
        {
            JumpPointer(Disk_Pointer, Add2BlockSqe(ParNode), 112 + 64);
            for (int i = 0; i < 32; i++)
            {
                Disk_Pointer << 0;
            }
        }

        /*将文件i结点的内容清除*/
        BitMapChange(Disk_Pointer, BlockSqe, 0);
        JumpPointer(Disk_Pointer, BlockSqe, 0);
        for (int i = 0; i < 240; i++)
        {
            Disk_Pointer << 0;
        }
        return;
    }
}

/*newfile*/
string FileNameTest(string FileName)
{
    string Right = "";
    if (FileName.length() == 0)
    {
        return "地址有误，创建失败!";
    }
    if (FileName.length() > 8)
    {
        return "文件名过长，创建失败！";
    }
    int CountPoint = 0;
    for (int i = 0; i < FileName.length(); i++)
    {
        if (FileName[i] == '.')
        {
            CountPoint++;
        }
        if (FileName[i] == '*' || FileName[i] == '/')
        {
            return "文件名格式错误，创建失败！";
        }
    }
    if (CountPoint != 1)
    {
        return "文件名格式错误，创建失败！";
    }
    return Right;
}

string FileAccTest(string Acc)
{
    string Right = "";
    if (Acc.length() != 6)
    {
        return "保护类型长度错误，创建失败!";
    }
    for (int i = 0; i < Acc.length(); i++)
    {
        if (Acc[i] != '0'&& Acc[i] != '1')
        {
            return "保护类型格式错误，创建失败!";
        }
    }
    return Right;
}

string newfileAddLocation(fstream& Disk_Pointer, string AddBuf)
{
    string ErrorReturn = "";
    string CurDirName;//用于暂时存储当前位置
    if (AddBuf.length() == 0)//如果地址为空，错误
    {
        return ErrorReturn;
    }

    /*判断所给地址是否为绝对地址，如果是，跳转到相应位置并返回需要创建的文件名，如果不是，直接返回需要创建的文件名*/
    if (AddBuf[0] == '/')
    {
        JumpPointer(Disk_Pointer, 101, 0);//从根结点开始检索
        /*输入指令的分隔符/进行预处理*/
        for (int i = 0; i < AddBuf.length(); i++)//先处理地址中的/符号
        {
            if (AddBuf[i] == '/')AddBuf[i] = ' ';//将/替换为空格，便于输入
        }

        istringstream AddStr(AddBuf);
        AddStr >> CurDirName;

        /*判断根地址是否有问题*/
        if (CurDirName != "cmy")//如果根地址有误
        {
            return ErrorReturn;
        }

        /*通过子结点不断进行定位*/
        while (1)//当输入字符串AddressBuf中还有内容
        {
            AddStr >> CurDirName;//输入下一个目录
            if (AddStr.eof())//当输入完一个目录之后到底输入底部，那么这时候这个名字就是对应的文件名
            {
                break;
            }
            int ChildBlockSqe = MatchDocName(Disk_Pointer, GetBlockSqe(Disk_Pointer), CurDirName);
            if (ChildBlockSqe)//如果找到了对应的子文件
            {
                if (FindDocType(Disk_Pointer, ChildBlockSqe) != 0)//如果子文件不是目录
                {
                    return ErrorReturn;
                }
                JumpPointer(Disk_Pointer, ChildBlockSqe, 0);//跳转到对应的位置
            }
            else//如果没有知道对应的子文件
            {
                return ErrorReturn;
            }
        }
    }
    else
    {
        for (int i = 1; i < AddBuf.length(); i++)//判断文件名中有没有/符号
        {
            if (AddBuf[i] == '/')
            {
                return ErrorReturn;
            }
        }
        CurDirName = AddBuf;
    }
    return CurDirName;
}

/*cat*/
int catAddLocation(fstream& Disk_Pointer, string AddBuf)
{
    string CurDirName;//用于暂时存储当前位置
    if (AddBuf.length() == 0)//如果地址为空，返回当前磁盘块序号
    {
        return 0;
    }

    if (AddBuf[0] == '/')
    {
        JumpPointer(Disk_Pointer, 101, 0);//从根结点开始检索
        /*输入指令的分隔符/进行预处理*/
        for (int i = 0; i < AddBuf.length(); i++)//先处理地址中的/符号
        {
            if (AddBuf[i] == '/')AddBuf[i] = ' ';//将/替换为空格，便于输入
        }

        istringstream AddStr(AddBuf);
        AddStr >> CurDirName;

        /*判断根地址是否有问题*/
        if (CurDirName != "cmy")//如果根地址有误
        {
            return 0;
        }
        /*通过子结点不断进行定位*/
        while (1)//当输入字符串AddressBuf中还有内容
        {
            AddStr >> CurDirName;//输入下一个目录
            int ChildBlockSqe = MatchDocName(Disk_Pointer, GetBlockSqe(Disk_Pointer), CurDirName);
            if (ChildBlockSqe)//如果找到了对应的子文件
            {
                JumpPointer(Disk_Pointer, ChildBlockSqe, 0);//跳转到对应的位置
            }
            else//如果没有知道对应的子文件
            {
                return 0;
            }
            if (AddStr.eof())//当输入完一个目录之后到底输入底部，结束
            {
                if (FindDocType(Disk_Pointer, GetBlockSqe(Disk_Pointer)) != 1)//如果该文件是一个目录
                {
                    return 0;
                }
                return GetBlockSqe(Disk_Pointer);
            }
        }
    }
    else
    {
        return 0;
    }
}

string GetFileContext(fstream& Disk_Pointer, int BlockSqe)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    JumpPointer(Disk_Pointer, BlockSqe, 0);
    string FileContext = "";
    char TempChar;
    int* TempOutputIntArr;
    for (int i = 0; i < 128; i++)
    {
        TempOutputIntArr = Output8Bit(Disk_Pointer);
        TempChar=EightInt2Char(TempOutputIntArr);
        FileContext += TempChar;
    }
    JumpPointer(Disk_Pointer, OriSqe, 0);
    return FileContext;
}

/*del*/
void delDoc(fstream& Disk_Pointer, int BlockSqe)
{
    int* DocContextAdd = FindDocContextNode(Disk_Pointer, BlockSqe);

    /*将文件结点的内容清除*/
    BitMapChange(Disk_Pointer, Add2BlockSqe(DocContextAdd), 0);
    JumpPointer(Disk_Pointer, Add2BlockSqe(DocContextAdd), 0);
    for (int i = 0; i < 1024; i++)
    {
        Disk_Pointer << 0;
    }

    /*将父结点的对应子结点地址清除*/
    int* ParNode = FindDocParNodeAdd(Disk_Pointer, BlockSqe);
    char* DocName = FindDocName(Disk_Pointer, BlockSqe);
    int** ParChildNode = FindDocChildNodeAdd(Disk_Pointer, Add2BlockSqe(ParNode));

    if (string(DocName) == string(FindDocName(Disk_Pointer, Add2BlockSqe(ParChildNode[0]))))
    {
        JumpPointer(Disk_Pointer, Add2BlockSqe(ParNode), 112);
        for (int i = 0; i < 32; i++)
        {
            Disk_Pointer << 0;
        }
    }
    else if (string(DocName) == string(FindDocName(Disk_Pointer, Add2BlockSqe(ParChildNode[1]))))
    {
        JumpPointer(Disk_Pointer, Add2BlockSqe(ParNode), 112 + 32);
        for (int i = 0; i < 32; i++)
        {
            Disk_Pointer << 0;
        }
    }
    else if (string(DocName) == string(FindDocName(Disk_Pointer, Add2BlockSqe(ParChildNode[2]))))
    {
        JumpPointer(Disk_Pointer, Add2BlockSqe(ParNode), 112 + 64);
        for (int i = 0; i < 32; i++)
        {
            Disk_Pointer << 0;
        }
    }

    /*将文件i结点的内容清除*/
    BitMapChange(Disk_Pointer, BlockSqe, 0);
    JumpPointer(Disk_Pointer, BlockSqe, 0);
    for (int i = 0; i < 240; i++)
    {
        Disk_Pointer << 0;
    }
    return;
}

string GetWindowsFileName(string path)
{
    for (int i = 0; i < path.length();i++)
    {
        if (path[i] == '\\')
        {
            path[i] = ' ';
        }
    }
    istringstream TempInputStr(path);
    string TempStr;
    while (!TempInputStr.eof())
    {
        TempInputStr >> TempStr;
    }
    return TempStr;
}

string CopyNewFile(fstream& Disk_Pointer,istringstream& Buf)
{
    int OriSqe = GetBlockSqe(Disk_Pointer);
    string newfileNameBuf;//用户存储输入内容中的地址部分
    string newfileAccBuf;
    string newfileContextBuf;
    Buf >> newfileNameBuf;
    Buf >> newfileAccBuf;
    Buf >> newfileContextBuf;

    string CurDirName = mdAddLocation(Disk_Pointer, newfileNameBuf);
    /*文件名格式检测*/
    string ReplyInfo = FileNameTest(CurDirName);
    if (ReplyInfo != "")
    {
        return ReplyInfo;
    }

    /*保护类型格式检测*/
    ReplyInfo = FileAccTest(newfileAccBuf);
    if (ReplyInfo != "")
    {
        return ReplyInfo;
    }

    int NewINodeBlockSqe = FindFreeAreaInINode(Disk_Pointer);
    int NewDocBlockSqe = FindFreeAreaInDoc(Disk_Pointer);

    /*判断文件系统是否还有空间*/
    if (!NewINodeBlockSqe)//如果文件系统汇总没有空间，创建失败
    {
        return "文件系统已无空闲空间，创建失败！";
    }
    if (!NewDocBlockSqe)//如果文件系统汇总没有空间，创建失败
    {
        return "文件系统已无空闲空间，创建失败！";
    }

    /*判断所创建文件名是否与子结点中文件名重复*/
    if (MatchDocName(Disk_Pointer, GetBlockSqe(Disk_Pointer), CurDirName))
    {
        return "文件名重复，创建失败！";
    }

    /*判断子结点是不是满了*/
    int FreeChildSqe = FindFreeChildNode(Disk_Pointer, GetBlockSqe(Disk_Pointer));
    if (FreeChildSqe)//没满，将子结点标记为已经使用，并输入32位地址
    {
        JumpPointer(Disk_Pointer, GetBlockSqe(Disk_Pointer), 112 + 32 * (FreeChildSqe - 1));//跳转到对应的子结点
        int* INode = BlockSqe2Add(NewINodeBlockSqe);
        INode[0] = 1;//将首位置为1，表示该子结点已经被使用
        InputIntArr(Disk_Pointer, INode, 32);//输入32位子结点
        JumpPointer(Disk_Pointer, GetBlockSqe(Disk_Pointer), 0);//跳回开头
    }
    else
    {
        return "当前文件的子文件已达上限，创建失败！";
    }

    /*对新文件进行初始化*/
    InputDocName(Disk_Pointer, NewINodeBlockSqe, CurDirName);//输入文件名
    InputParNode(Disk_Pointer, NewINodeBlockSqe, BlockSqe2Add(GetBlockSqe(Disk_Pointer)));//输入父结点地址
    InputAcc(Disk_Pointer, NewINodeBlockSqe, newfileAccBuf);//输入文件保护类型
    InputFileContext(Disk_Pointer, NewINodeBlockSqe, NewDocBlockSqe, newfileContextBuf);//输入文件内容
    InputFileType(Disk_Pointer, NewINodeBlockSqe, 1);//输入文件类型
    BitMapChange(Disk_Pointer, NewINodeBlockSqe, 1);
    BitMapChange(Disk_Pointer, NewDocBlockSqe, 1);
    JumpPointer(Disk_Pointer, OriSqe, 0);
    return "文件创建成功！";
}

void ConsistencyCheck(fstream& Disk_Pointer,int BlockSqe)
{
    JumpPointer(Disk_Pointer, BlockSqe, 0);
    int FileType = FindDocType(Disk_Pointer, BlockSqe);
    BitMapChange(Disk_Pointer, BlockSqe, 1);//把该文件对应的位图比特置为1
    if (FileType == 1)//如果是一个文件
    {
        BitMapChange(Disk_Pointer, Add2BlockSqe(FindDocContextNode(Disk_Pointer, BlockSqe)), 1);//把文件内容对应的位图比特置为1
        return;
    }
    else
    {
        char TempChar;
        JumpPointer(Disk_Pointer, BlockSqe,112);
        Disk_Pointer >> TempChar;
        if (TempChar == '1')
        {
            JumpPointer(Disk_Pointer, BlockSqe, 112);
            int* ChildNodeAdd = Output32Bit(Disk_Pointer);
            ConsistencyCheck(Disk_Pointer, Add2BlockSqe(ChildNodeAdd));
        }

        JumpPointer(Disk_Pointer, BlockSqe, 144);
        Disk_Pointer >> TempChar;
        if (TempChar == '1')
        {
            JumpPointer(Disk_Pointer, BlockSqe, 144);
            int* ChildNodeAdd = Output32Bit(Disk_Pointer);
            ConsistencyCheck(Disk_Pointer, Add2BlockSqe(ChildNodeAdd));
        }

        JumpPointer(Disk_Pointer, BlockSqe, 176);
        Disk_Pointer >> TempChar;
        if (TempChar == '1')
        {
            JumpPointer(Disk_Pointer, BlockSqe, 176);
            int* ChildNodeAdd = Output32Bit(Disk_Pointer);
            ConsistencyCheck(Disk_Pointer, Add2BlockSqe(ChildNodeAdd));
        }
        return;
    }
}