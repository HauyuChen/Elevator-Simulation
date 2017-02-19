/******************************************************
** Á´Ê½Õ»
******************************************************/
template<class T>
struct LinkNode
{
	T data;
	LinkNode<T> *next;
};

template<class T>
class LinkedStack
{
	private:
		LinkNode<T> *top;
	public:
		LinkedStack(){top=NULL;}
		~LinkedStack(){ClearStack();};
		void Push(T x);
		T Pop();
		bool StackIsEmpty();
		void ClearStack();
		void StackTranverse();
};

template<class T>
void LinkedStack<T>::Push(T x)
{
	LinkNode<T> *p = new LinkNode<T>;
	p->data = x;
	p->next = top;
	top = p;
}

template<class T>
T LinkedStack<T>::Pop()
{
	T x;
	if(StackIsEmpty()) throw"Õ»Îª¿Õ£¡";
	LinkNode<T> *p = top;
	top = p->next;
	x = p->data;
	delete p;
	return x;
}

template<class T>
void LinkedStack<T>::ClearStack()
{
	LinkNode<T> *p;
	while(top!=NULL)
	{
		p = top;
		top = top->next;
		delete p;
	}
}

template<class T>
bool LinkedStack<T>::StackIsEmpty()
{
	return (top==NULL)?true:false;
}

template<class T>
void LinkedStack<T>::StackTranverse()
{
	LinkNode<T> *p = top;
	while(p!=NULL)
	{
		cout<<p->data;
		p = p->next;
	}
}














