/********************************************************
** 链式队列
********************************************************/
template<class T>
struct LinkNodes
{
	T data;
	LinkNodes<T> *next;
};

template<class T>
class LinkedQueue
{
	private:
		LinkNodes<T> *front;
		LinkNodes<T> *rear;
	public:
		LinkedQueue();
		~LinkedQueue(){ClearQueue();};
		void EnQueue(T x);
		T DeQueue();
		bool QueueIsEmpty();
		void ClearQueue();
		void QueueTranverse();
};

template<class T>
LinkedQueue<T>::LinkedQueue()
{
	front = new LinkNodes<T>;
	front->next = NULL;
	rear = front;
}

template<class T>
void LinkedQueue<T>::EnQueue(T x)
{
	LinkNodes<T> *p = new LinkNodes<T>;
	p->data = x;
	p->next = NULL;
	rear->next = p;
	rear = p;
}

template<class T>
T LinkedQueue<T>::DeQueue()
{
	T x;
	if(rear==front) throw"队列为空！";
	LinkNodes<T> *p = front->next;
	x = p->data;
	front->next = p->next;
	if(rear==p)	rear=front;
	delete p;
	return x;
}

template<class T>
void LinkedQueue<T>::ClearQueue()
{
	LinkNodes<T> *p;
	while(front!=NULL)
	{
		p = front;
		front = front->next;
		delete p;
	}
}

template<class T>
bool LinkedQueue<T>::QueueIsEmpty()
{
	return (front==rear)?true:false;
}

template<class T>
void LinkedQueue<T>::QueueTranverse()
{
	LinkNodes<T> *p = front->next;
	while(p!=NULL)
	{
		cout<<p->data;
		p = p->next;
	}
}













