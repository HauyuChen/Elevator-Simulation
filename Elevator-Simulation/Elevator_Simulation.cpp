#include "stdafx.h"

#define TIME 100
#define MAXPSG 5

/******************************************************************************
** ȫ�ֱ�������
******************************************************************************/
enum states	//���ݵ�����״̬
{
	up,		//����
	down,	//�½�
	idle	//����
};
states nowstate = idle;		//���ݵ�ǰ��״̬

int nowfloor = 1;			//���ݵ�ǰ��¥��

int req_up[6];				//��������
int req_down[6];			//��������
int req_floor[6];			//�����¥��

long timesum;
long delay;					//������ʱ
long sum = 1;
long nexttime;				//��һ���˿ͳ��ֵ�ʱ��
long now_stop;				//��ֹ��ʼʱ��
int move;					//���ݶ�����־
int next_psg_time;			//��һ���˳��ֵ�ʱ����
int psg_num;				//�����ڵ�����
LinkedStack<int> time_save;	//����ʱ�䱣�溯���ݹ���ù����ж�ʱ��ı���

/*****************************************************************************
** ����ԭ��
*****************************************************************************/
void Init();			//��ʼ������
int Wait(long t);		//��ʱ����
void TimePause();
void TimeResume();
int Elv_OpenDoor();		//���ݿ���
int Elv_CloseDoor();	//���ݹ���
int Elv_PsgIn();		//�˿ͽ������
int Elv_PsgOut();		//�˿��߳�����
int Elv_Up();			//��������
int Elv_Down();			//�����½�
int Elv_Stop();			//���ݾ�ֹ
int Elv_Work();			//���ݹ���
int Elv_States();		//����״̬
int Elv_Control();		//���ݿ���
int New_Psg();			//�����˿�
int Psg_Infm();			//�˿���Ϣ

/*************************************************************************
** ��ʼ��
*************************************************************************/
//��ʼ��
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

//�������ܣ���ʾ��ӭ����
void Welcome()
{
	cout<<endl<<endl;
	cout<<"                   *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*"<<endl;
	cout<<"                    *          ��ӭʹ�õ���ģ��ϵͳ         *"<<endl;
	cout<<"                   *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*"<<endl<<endl;
	cout<<"                             ----------------------"<<endl;
	cout<<"                             *      ��ܰ��ʾ      *"<<endl;
	cout<<"--------------------------------------------------------------------------------"<<endl;
	cout<<"        ��ϵͳּ��ģ����׵ĵ������й��ܣ����幦�����£�"<<endl;
	cout<<"            �ٳ˿Ϳ���������¥���ڽ�����߳�����"<<endl;
	cout<<"            ��ͨ��������һ�˿͵���ʱ�����������˿�"<<endl;
	cout<<"            �۶�̬��ʾ���ݵ�״̬��Ϣ"<<endl;
	cout<<"        ����"<<endl;
	cout<<"           ����ʾ�������˿͡�ʱ������1 5 1000"<<endl;
	cout<<"           ��ʾ�ó˿ʹ�һ¥������ݣ�Ҫȥ��¥������ʱ��1000t�����һ���³˿͵���"<<endl;
	cout<<"--------------------------------------------------------------------------------"<<endl;
	cout<<endl<<endl;
}

/*************************************************************************
** ʱ��ģ��
*************************************************************************/
//�������ܣ���ʱ����
int Wait(long t)
{
	clock_t end;
	end = clock()-delay;
	while(clock() < end+delay+t*TIME)	//����ֻ�õ�ǰʱ����ɵĴ���
	{
		if((clock()-delay) > nexttime)	//�����³˿͵���ʱ
		{
			New_Psg();	//�����˿�
		}
	}
	return 0;
}

//�������ܣ���ͣ��ʱ
void TimePause()
{
	time_save.Push(clock());
	timesum = clock();
}

//�������ܣ��ָ���ʱ
void TimeResume()
{
	delay += clock()-time_save.Pop();	//����˿���Ϣʱ��ռ�õ�ʱ��
}

/*************************************************************************
** �˿�ģ��
*************************************************************************/
//�ࣺ�˿���Ķ���
class psg
{
	public:
		int ID;			//�˿͵�����
		int infloor;	//�˿ʹ��ĸ�¥��������
		int outfloor;	//�˿ʹ��ĸ�¥�������
		void In_Elv();	//�˿ͽ������
		void onwait();	//�˿���¥���ȴ�����
		void getout();	//�˿��߳�����
		//�������������ջ�Ͷ��еı���
		friend ostream& operator << (ostream& output,psg& c)
		{
			output<<c.ID<<"�� ";
			return output;
		}
};

//���������˿Ͷ���
psg m;
psg n;

//�ȴ�����
LinkedQueue<psg> wait_psg[5];
//����ջ
LinkedStack<psg> elv_psg[5];


//�˿���ĳ�Ա�������˿ͽ������
void psg::In_Elv()
{
	elv_psg[outfloor-1].Push(*this);
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"���˿͡�"<<ID<<"�ų˿ʹ�"<<infloor<<"¥������ݡ�"<<endl;
	req_floor[outfloor] = 1;
}

//�˿���ĳ�Ա�������˿�������¥��ȴ�
void psg::onwait()
{
	wait_psg[infloor-1].EnQueue(*this);
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"���˿͡�"<<ID<<"�ų˿ͽ���"<<infloor<<"¥�ĵȴ����С�"<<endl;
}


//�˿���ĳ�Ա�������˿ͳ�����
void psg::getout()
{
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"���˿͡�"<<ID<<"�ų˿ͳ����ݡ�"<<endl;
}

//�������ܣ�����һ���˿�
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
		req_down[m.infloor] = 1;	//Ҫ������
	}
	else
	{
		req_up[m.infloor] = 1;		//Ҫ������
	}
	m.onwait();	//�³˿ͽ���ȴ�����
	return 0;
}

//�������ܣ��˿���Ϣ����
int Psg_Infm()
{
	int timeflag = 0;	//ʱ��ָ���־
	cout<<"�������˿͡������룺����¥�㡢Ŀ��¥�㡢��һ�˿͵���ʱ�������磺1 5 500��\n";
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
			cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"����������"<<"����¥���������������"<<endl;
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
** ����ģ��
************************************************************************/
//�������ܣ���ʾ����״̬
int Elv_States()
{
	int i;
	cout<<"******************************** ����״̬��Ϣ *********************************"<<endl;
	cout<<"**********************-----------------------------------**********************"<<endl;
	cout<<"************-------------------- �ִ�¥�㣺"<<nowfloor<<" --------------------**************"<<endl;

	cout<<"                            ح------------------ح                             "<<endl;
	cout<<"                            ح ǰ����¥��ĳ˿� ح                             "<<endl;
	cout<<"                            ح------------------ح                             "<<endl;
	for(i=5;i>=1;i--)
	{
		cout<<"                            ح";
		if(i==nowfloor)
		{
			cout<<"  ��";
		}
		else
		{
			cout<<"    ";
		}
		cout<<" ��"<<i<<"�㣺";
		elv_psg[i-1].StackTranverse();	//����ÿһ������Ӧ��ջ�����Ƿ��г˿�
		cout<<"      ح"<<endl;
	}
	cout<<endl;

	cout<<"                            ح------------------ح                             "<<endl;
	cout<<"                            ح ��¥��ȴ��ĳ˿� ح                             "<<endl;
	cout<<"                            ح------------------ح                             "<<endl;
	for(i=5;i>=1;i--)
	{
		cout<<"                            ح";
		if(i==nowfloor)
		{
			cout<<"  ��";
		}
		else
		{
			cout<<"    ";
		}
		cout<<" ��"<<i<<"�㣺";
		wait_psg[i-1].QueueTranverse();	//����ÿһ������Ӧ�Ķ��У����Ƿ��г˿�
		cout<<"      ح"<<endl;
	}
	cout<<"-------------------------------------------------------------------------------"<<endl;

	return 0;
}

//�������ܣ����ݿ���
int Elv_Control()
{
	if((clock()-delay) >= nexttime)	//���г˿͵���ʱ
	{
		New_Psg();	//�����˿�
		return 0;
	}
	//���ݿ��ŵ�����
	if((req_floor[nowfloor]!=0) || req_up[nowfloor]!=0 || req_down[nowfloor]!=0 || (elv_psg[nowfloor-1].StackIsEmpty()==0) || wait_psg[nowfloor-1].QueueIsEmpty()==0)
	{
		if((psg_num==MAXPSG) && (elv_psg[nowfloor-1].StackIsEmpty()==1))
		{
			Elv_Work();
		}
		else
		{
			Elv_OpenDoor();	//���ݿ���
			Elv_PsgOut();	//�˿��߳�����
			Elv_PsgIn();	//�˿ͽ������
			Elv_CloseDoor();	//���ݹ���
			Elv_Work();	//���ݿ�ʼ����
		}
	}
	else
	{
		Elv_Work();	//���ݿ�ʼ����
	}
	if(move==0)
	{
		req_floor[nowfloor] = 0;
		if((clock()-now_stop)>250*TIME)	//���ݵȴ�ʱ�䳬��300tʱ���ص�һ��
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

//�������ܣ����ݴ�
int Elv_OpenDoor()
{
	Wait(20);
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"�����������С�"<<"���ڿ���..."<<endl;
	return 0;
}

//�������ܣ����ݹ���
int Elv_CloseDoor()
{
	Wait(20);
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"�����������С�"<<"���ڹ���..."<<endl;
	return 0;
}

//�������ܣ��˿ͽ������
int Elv_PsgIn()
{
	while(!wait_psg[nowfloor-1].QueueIsEmpty() && psg_num <MAXPSG)	//����ȴ��������г˿�
	{
		n = wait_psg[nowfloor-1].DeQueue();
		Wait(30);
		n.In_Elv();
		psg_num++;
	}
	return 0;
}

//�������ܣ��˿��߳�����
int Elv_PsgOut()
{
	while(!elv_psg[nowfloor-1].StackIsEmpty())	//����������г˿�
	{
		n = elv_psg[nowfloor-1].Pop();
		Wait(30);
		n.getout();
		psg_num--;
	}
	return 0;
}

//�������ܣ���������
int Elv_Up()
{
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"�����������С�"<<"��������..."<<endl;
	Wait(40);
	req_floor[nowfloor] = 0;
	nowfloor++;
	req_up[nowfloor-1] = 0;
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"�����������С�"<<"�����������˵�"<<nowfloor<<"��"<<endl;
	req_floor[nowfloor] = 0;
	move = 1;
	return 0;
}

//�������ܣ������½�
int Elv_Down()
{
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"�����������С�"<<"�����½�..."<<endl;
	Wait(40);
	req_floor[nowfloor] = 0;
	nowfloor--;
	req_down[nowfloor+1] = 0;
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"�����������С�"<<"�����½����˵�"<<nowfloor<<"��"<<endl;
	req_floor[nowfloor] = 0;
	move = 1;
	return 0;
}

//�������ܣ����ݾ�ֹ
int Elv_Stop()
{
	cout<<(clock()-delay)/TIME<<","<<nexttime/TIME<<"�����ݾ�ֹ��"<<"����������¥���ǣ�"<<nowfloor<<endl;
	if(move)
	{
		now_stop = clock();
	}
	move = 0;
	return 0;
}

//�������ܣ����ݹ���״̬�仯
int Elv_Work()
{
	system("cls");	//����
	Elv_States();	//��ʾ����״̬

	int upflag;		//�����Ƿ�Ӧ��������־
	int downflag;	//�����Ƿ�Ӧ���½���־
	int nowflag = nowfloor;	//�����������ڵ�¥���־

	if(req_up[nowflag]==1)	//�����ǰ¥�������ϵ�����ʱ
	{
		upflag = 1;	//����Ӧ��������
	}
	else
	{
		for(upflag=0,nowflag=nowfloor+1;nowflag<=5;nowflag++)
		{
			//�жϵ�ǰ¥�������¥���Ƿ������󣬵�������ʱ
			if(req_floor[nowflag]==1 || req_up[nowflag]==1 ||req_down[nowflag]==1 || (elv_psg[nowflag-1].StackIsEmpty()==0) || wait_psg[nowflag-1].QueueIsEmpty()==0)
			{
				upflag = 1;	//����Ӧ��������
				break;
			}
		}
	}

	nowflag = nowfloor;
	if(req_down[nowflag]==1)		//�����ǰ¥�������µ�����ʱ
	{
		downflag = 1;	//����Ӧ��������
	}
	else
	{
		for(downflag=0,nowflag=nowfloor-1;nowflag>=1;nowflag--)
		{
			//�жϵ�ǰ¥�������¥���Ƿ������󣬵�������ʱ
			if(req_floor[nowflag]==1 || req_up[nowflag]==1 || req_down[nowflag]==1 || (elv_psg[nowflag-1].StackIsEmpty()==0) || wait_psg[nowflag-1].QueueIsEmpty()==0)
			{
				downflag = 1;	//����Ӧ��������
				break;
			}
		}
	}

	if(nowstate == up)
	{
		if(nowfloor==5 || (upflag==0&&downflag==1))	//����������תΪ���µ�����
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
			if(nowfloor==1 || (downflag==0&&upflag==1))	//����������תΪ���ϵ�����
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
** ������
*************************************************************************************/
int main()
{
	timesum = clock();
	Init();		//��ʼ������
	Welcome();
	system("pause");	//��ͣ

	while(1)
	{
		system("cls");	//����
		Elv_States();	//��ʾ����״̬
		while((clock()-delay)<nexttime)	//���ݿ���
		{
			if(next_psg_time>10)
			{
				Wait(10);
			}
			Elv_Control();
		}
		New_Psg();	//����˿���Ϣ
	}

	return 0;
}


















