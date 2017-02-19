#include "stdafx.h"

#define TIME 100
#define MAXPSG 5

/******************************************************************************
** 全局变量定义
******************************************************************************/
enum states	//电梯的三个状态
{
	up,		//上升
	down,	//下降
	idle	//空闲
};
states nowstate = idle;		//电梯当前的状态

int nowfloor = 1;			//电梯当前的楼层

int req_up[6];				//请求向上
int req_down[6];			//请求向下
int req_floor[6];			//请求的楼层

long timesum;
long delay;					//输入延时
long sum = 1;
long nexttime;				//下一个乘客出现的时刻
long now_stop;				//静止开始时间
int move;					//电梯动作标志
int next_psg_time;			//下一个人出现的时间间隔
int psg_num;				//电梯内的人数
LinkedStack<int> time_save;	//用于时间保存函数递归调用过程中对时间的保存

/*****************************************************************************
** 函数原型
*****************************************************************************/
void Init();			//初始化函数
int Wait(long t);		//延时函数
void TimePause();
void TimeResume();
int Elv_OpenDoor();		//电梯开门
int Elv_CloseDoor();	//电梯关门
int Elv_PsgIn();		//乘客进入电梯
int Elv_PsgOut();		//乘客走出电梯
int Elv_Up();			//电梯上升
int Elv_Down();			//电梯下降
int Elv_Stop();			//电梯静止
int Elv_Work();			//电梯工作
int Elv_States();		//电梯状态
int Elv_Control();		//电梯控制
int New_Psg();			//新增乘客
int Psg_Infm();			//乘客信息

/*************************************************************************
** 初始化
*************************************************************************/
//初始化
void Init()
{
	int i = 6;
	while(i)
	{
		req_up[i] = 0;
		req_down[i] = 0;
		req_floor[i] = 0;
		i--;
	}
}

//函数功能：显示欢迎界面
void Welcome()
{
	cout<<endl<<endl;
	cout<<"                   *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*"<<endl;
	cout<<"                    *          欢迎使用电梯模拟系统         *"<<endl;
	cout<<"                   *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*"<<endl<<endl;
	cout<<"                             ----------------------"<<endl;
	cout<<"                             *      温馨提示      *"<<endl;
	cout<<"--------------------------------------------------------------------------------"<<endl;
	cout<<"        本系统旨在模拟简易的电梯运行功能，具体功能如下："<<endl;
	cout<<"            ①乘客可以在任意楼层内进入和走出电梯"<<endl;
	cout<<"            ②通过设置下一乘客到来时间间隔来新增乘客"<<endl;
	cout<<"            ③动态显示电梯的状态信息"<<endl;
	cout<<"        例："<<endl;
	cout<<"           当提示【新增乘客】时，输入1 5 1000"<<endl;
	cout<<"           表示该乘客从一楼进入电梯，要去五楼，程序时间1000t后会有一名新乘客到来"<<endl;
	cout<<"--------------------------------------------------------------------------------"<<endl;
	cout<<endl<<endl;
}

/*************************************************************************
** 时间模块
*************************************************************************/
//函数功能：延时控制
int Wait(long t)
{
	clock_t end;
	end = clock()-delay;
	while(clock() < end+delay+t*TIME)	//修正只用当前时刻造成的错误
	{
		if((clock()-delay) > nexttime)	//当有新乘客到来时
		{
			New_Psg();	//新增乘客
		}
	}
	return 0;
}

//函数功能：暂停计时
void TimePause()
{
	time_save.Push(clock());
	timesum = clock();
}

//函数功能：恢复计时
void TimeResume()
{
	delay += clock()-time_save.Pop();	//输入乘客信息时所占用的时间
}

/*************************************************************************
** 乘客模块
*************************************************************************/
//类：乘客类的定义
class psg
{
	public:
		int ID;			//乘客的数量
		int infloor;	//乘客从哪个楼层进入电梯
		int outfloor;	//乘客从哪个楼层出电梯
		void In_Elv();	//乘客进入电梯
		void onwait();	//乘客在楼道等待电梯
		void getout();	//乘客走出电梯
		//重载流输出用于栈和队列的遍历
		friend ostream& operator << (ostream& output,psg& c)
		{
			output<<c.ID<<"号 ";
			return output;
		}
};

//定义两个乘客对象
psg m;
psg n;

//等待队列
LinkedQueue<psg> wait_psg[5];
//电梯栈
LinkedStack<psg> elv_psg[5];


//乘客类的成员函数：乘客进入电梯
void psg::In_Elv()
{
	elv_psg[outfloor-1].Push(*this);
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"【乘客】"<<ID<<"号乘客从"<<infloor<<"楼进入电梯。"<<endl;
	req_floor[outfloor] = 1;
}

//乘客类的成员函数：乘客在所在楼层等待
void psg::onwait()
{
	wait_psg[infloor-1].EnQueue(*this);
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"【乘客】"<<ID<<"号乘客进入"<<infloor<<"楼的等待队列。"<<endl;
}


//乘客类的成员函数：乘客出电梯
void psg::getout()
{
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"【乘客】"<<ID<<"号乘客出电梯。"<<endl;
}

//函数功能：新增一名乘客
int New_Psg()
{
	m.ID = sum;
	sum++;
	Psg_Infm();
	if(nexttime>clock()-delay)
	{
		nexttime += nexttime+next_psg_time*TIME;
	}
	else
	{
		nexttime = clock()-delay+next_psg_time*TIME;
	}
	if(m.infloor>m.outfloor)
	{
		req_down[m.infloor] = 1;	//要求向下
	}
	else
	{
		req_up[m.infloor] = 1;		//要求向上
	}
	m.onwait();	//新乘客进入等待队列
	return 0;
}

//函数功能：乘客信息输入
int Psg_Infm()
{
	int timeflag = 0;	//时间恢复标志
	cout<<"【新增乘客】请输入：所在楼层、目标楼层、下一乘客到来时间间隔（如：1 5 500）\n";
	if(timeflag==0)
	{
		TimePause();
	}
	timeflag = 1;

	while(cin>>m.infloor>>m.outfloor>>next_psg_time)
	{
		if(m.infloor==0 && m.outfloor==0 && next_psg_time==0)
		{
			int nowdelay;
			nowdelay = delay;
			cout<<" "<<endl;
			Elv_States();
			Psg_Infm();
		}
		if(m.infloor>=6||m.infloor<=0|| m.outfloor>=6|| m.outfloor<=0||m.infloor==m.outfloor)
		{
			cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"【输入有误】"<<"输入楼层错误！请重新输入"<<endl;
			continue;
		}
		else
		{
			break;
		}
	}
	if(timeflag!=0)
	{
		TimeResume();
	}
	timeflag = 0;
	return 0;
}


/************************************************************************
** 电梯模块
************************************************************************/
//函数功能：显示电梯状态
int Elv_States()
{
	int i;
	cout<<"******************************** 电梯状态信息 *********************************"<<endl;
	cout<<"**********************-----------------------------------**********************"<<endl;
	cout<<"************-------------------- 现处楼层："<<nowfloor<<" --------------------**************"<<endl;

	cout<<"                            丨------------------丨                             "<<endl;
	cout<<"                            丨 前往各楼层的乘客 丨                             "<<endl;
	cout<<"                            丨------------------丨                             "<<endl;
	for(i=5;i>=1;i--)
	{
		cout<<"                            丨";
		if(i==nowfloor)
		{
			cout<<"  ◆";
		}
		else
		{
			cout<<"    ";
		}
		cout<<" 第"<<i<<"层：";
		elv_psg[i-1].StackTranverse();	//遍历每一层所对应的栈，看是否有乘客
		cout<<"      丨"<<endl;
	}
	cout<<endl;

	cout<<"                            丨------------------丨                             "<<endl;
	cout<<"                            丨 各楼层等待的乘客 丨                             "<<endl;
	cout<<"                            丨------------------丨                             "<<endl;
	for(i=5;i>=1;i--)
	{
		cout<<"                            丨";
		if(i==nowfloor)
		{
			cout<<"  ◆";
		}
		else
		{
			cout<<"    ";
		}
		cout<<" 第"<<i<<"层：";
		wait_psg[i-1].QueueTranverse();	//遍历每一层所对应的队列，看是否有乘客
		cout<<"      丨"<<endl;
	}
	cout<<"-------------------------------------------------------------------------------"<<endl;

	return 0;
}

//函数功能：电梯控制
int Elv_Control()
{
	if((clock()-delay) >= nexttime)	//当有乘客到来时
	{
		New_Psg();	//新增乘客
		return 0;
	}
	//电梯开门的条件
	if((req_floor[nowfloor]!=0) || req_up[nowfloor]!=0 || req_down[nowfloor]!=0 || (elv_psg[nowfloor-1].StackIsEmpty()==0) || wait_psg[nowfloor-1].QueueIsEmpty()==0)
	{
		if((psg_num==MAXPSG) && (elv_psg[nowfloor-1].StackIsEmpty()==1))
		{
			Elv_Work();
		}
		else
		{
			Elv_OpenDoor();	//电梯开门
			Elv_PsgOut();	//乘客走出电梯
			Elv_PsgIn();	//乘客进入电梯
			Elv_CloseDoor();	//电梯关门
			Elv_Work();	//电梯开始工作
		}
	}
	else
	{
		Elv_Work();	//电梯开始工作
	}
	if(move==0)
	{
		req_floor[nowfloor] = 0;
		if((clock()-now_stop)>250*TIME)	//电梯等待时间超过300t时返回第一层
		{
			if(nowfloor!=1)
			{
				req_floor[1] = 1;
			}
		}
	}
	if((elv_psg[nowfloor-1].StackIsEmpty()==1) || wait_psg[nowfloor-1].QueueIsEmpty()==1)
	{
		req_up[nowfloor] = 0;
		req_down[nowfloor] = 0;
	}
	return 0;
}

//函数功能：电梯打开
int Elv_OpenDoor()
{
	Wait(20);
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"【电梯运行中】"<<"正在开门..."<<endl;
	return 0;
}

//函数功能：电梯关门
int Elv_CloseDoor()
{
	Wait(20);
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"【电梯运行中】"<<"正在关门..."<<endl;
	return 0;
}

//函数功能：乘客进入电梯
int Elv_PsgIn()
{
	while(!wait_psg[nowfloor-1].QueueIsEmpty() && psg_num <MAXPSG)	//如果等待队列中有乘客
	{
		n = wait_psg[nowfloor-1].DeQueue();
		Wait(30);
		n.In_Elv();
		psg_num++;
	}
	return 0;
}

//函数功能：乘客走出电梯
int Elv_PsgOut()
{
	while(!elv_psg[nowfloor-1].StackIsEmpty())	//如果电梯内有乘客
	{
		n = elv_psg[nowfloor-1].Pop();
		Wait(30);
		n.getout();
		psg_num--;
	}
	return 0;
}

//函数功能：电梯上升
int Elv_Up()
{
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"【电梯运行中】"<<"正在上升..."<<endl;
	Wait(40);
	req_floor[nowfloor] = 0;
	nowfloor++;
	req_up[nowfloor-1] = 0;
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"【电梯运行中】"<<"现在上升到了第"<<nowfloor<<"层"<<endl;
	req_floor[nowfloor] = 0;
	move = 1;
	return 0;
}

//函数功能：电梯下降
int Elv_Down()
{
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"【电梯运行中】"<<"正在下降..."<<endl;
	Wait(40);
	req_floor[nowfloor] = 0;
	nowfloor--;
	req_down[nowfloor+1] = 0;
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"【电梯运行中】"<<"现在下降到了第"<<nowfloor<<"层"<<endl;
	req_floor[nowfloor] = 0;
	move = 1;
	return 0;
}

//函数功能：电梯静止
int Elv_Stop()
{
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"【电梯静止】"<<"现在所处的楼层是："<<nowfloor<<endl;
	if(move)
	{
		now_stop = clock();
	}
	move = 0;
	return 0;
}

//函数功能：电梯工作状态变化
int Elv_Work()
{
	system("cls");	//清屏
	Elv_States();	//显示电梯状态

	int upflag;		//电梯是否应该上升标志
	int downflag;	//电梯是否应该下降标志
	int nowflag = nowfloor;	//电梯现在所在的楼层标志

	if(req_up[nowflag]==1)	//如果当前楼层有向上的请求时
	{
		upflag = 1;	//电梯应该向上走
	}
	else
	{
		for(upflag=0,nowflag=nowfloor+1;nowflag<=5;nowflag++)
		{
			//判断当前楼层上面的楼层是否有请求，当有请求时
			if(req_floor[nowflag]==1 || req_up[nowflag]==1 ||req_down[nowflag]==1 || (elv_psg[nowflag-1].StackIsEmpty()==0) || wait_psg[nowflag-1].QueueIsEmpty()==0)
			{
				upflag = 1;	//电梯应该向上走
				break;
			}
		}
	}

	nowflag = nowfloor;
	if(req_down[nowflag]==1)		//如果当前楼层有向下的请求时
	{
		downflag = 1;	//电梯应该向下走
	}
	else
	{
		for(downflag=0,nowflag=nowfloor-1;nowflag>=1;nowflag--)
		{
			//判断当前楼层下面的楼层是否有请求，当有请求时
			if(req_floor[nowflag]==1 || req_up[nowflag]==1 || req_down[nowflag]==1 || (elv_psg[nowflag-1].StackIsEmpty()==0) || wait_psg[nowflag-1].QueueIsEmpty()==0)
			{
				downflag = 1;	//电梯应该向下走
				break;
			}
		}
	}

	if(nowstate == up)
	{
		if(nowfloor==5 || (upflag==0&&downflag==1))	//电梯由向上转为向下的条件
		{
			if(nowfloor==5 && downflag==0)
			{
				Elv_Stop();
			}
			else
			{
				nowstate = down;
				Elv_Down();
			}
		}
		else
		{
			if(downflag==0 && upflag==0)
			{
				Elv_Stop();
				nowstate = idle;
			}
			else
			{
				Elv_Up();
			}
		}
	}
	else if(nowstate == down)
		{
			if(nowfloor==1 || (downflag==0&&upflag==1))	//电梯由向下转为向上的条件
			{
				if(downflag==0 && upflag==0)
				{
					Elv_Stop();
				}
				else
				{
					nowstate = up;
					Elv_Up();
				}
			}
			else
			{
				if(upflag==0 && downflag==0)
				{
					Elv_Stop();
					nowstate = idle;
				}
				else
				{
					Elv_Down();
				}
			}
		}
		else
		{
			if(upflag==0 && downflag==0)
			{
				nowstate = idle;
				Elv_Stop();
			}
			else
			{
				if(upflag==1)
				{
					nowstate = up;
					Elv_Up();
				}
				else
				{
					nowstate = down;
					Elv_Down();
				}
			}
		}
	
	return 0;
}


/*************************************************************************************
** 主函数
*************************************************************************************/
int main()
{
	timesum = clock();
	Init();		//初始化函数
	Welcome();
	system("pause");	//暂停

	while(1)
	{
		system("cls");	//清屏
		Elv_States();	//显示电梯状态
		while((clock()-delay)<nexttime)	//电梯控制
		{
			if(next_psg_time>10)
			{
				Wait(10);
			}
			Elv_Control();
		}
		New_Psg();	//输入乘客信息
	}

	return 0;
}


















