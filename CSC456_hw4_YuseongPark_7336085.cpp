#include<iostream>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cstdlib>
#include<ctime>
#include<sys/time.h>
#include<time.h>

using namespace std;

struct processlist
{
	int pid;
	int size;
	int *pageTable;
	int *pageNum;
	struct processlist *link;


};

//declaration of FIFO algorithm structure.
struct Fifo
{
	int pid;
	int size;
	int frame;
	int time;
	int page;
	struct Fifo *next;
	struct Fifo *pre;
};

//declaration of LRU algorithm structure
struct Lru
{
	int pid;
	int size;
	int frame;
	int page;
	struct Lru *next;
	struct Lru *pre; 
	
};

int PF=0;
int replace=0;
int Phy_size;
char *Phy_mem;
char *freeFrameList;
struct processlist *front=NULL;
struct processlist *rear=NULL;

struct Fifo *head = NULL;
struct Fifo *tail = NULL;

struct Lru *top =NULL;
struct Lru *bottom=NULL;


struct Fifo *ManageFifo1(int pid, int page);



//tracking into LRU function
struct processlist *tracking(int pid)
{
	struct processlist *search;
	search = front;
	while(search->link !=NULL)
	{
		if(search ->pid==pid)
		{
			break;
		}

		else
		{
			search=search->link;
		}
	}

	if(search->pid==pid)
	{
		return search;
	}
	else
	{
		return NULL;
	}

}

//paste Fifo and Lru
void Record(int pid, int page, int frame)
{
	struct Lru *Record1=NULL;
	struct Fifo *Record2=NULL;


	struct Fifo *f=NULL;

	clock_t t;
	
	//Fifo
	if(replace == 0)
	{
		t = clock();
	
		Record2= new struct Fifo;
		Record2->pid= pid;
		Record2->page=page;
		Record2->frame = frame;
		Record2->time=((double)t)/CLOCKS_PER_SEC;
		Record2->next=NULL;
	}

	//LRU
	else //if(replace ==1)
	{
		Record1=new struct Lru();
		Record1->pid=pid;
		Record1->page=page;
		Record1->frame=frame;
		Record1->next=NULL;
	}
	

	//Fifo
	if(replace ==0)
	{
		if(head==NULL)
		{
			head =Record2;
			tail = Record2;
		}
		else 
		{
			tail->next=Record2;
			tail=Record2;
		}
	}
	//LRU
	else //if(replace==1)
	{
		if(top==NULL)
		{
			top=Record1;
			bottom=Record1;
		}
		else
		{
			bottom->next=Record1;
			bottom=Record1;
		}
	}
}




void memoryManager(int memSize, int frame)
{
	Phy_size = memSize;

	Phy_mem = new char[memSize];	
	freeFrameList = new char[memSize];

	//initialization.
	//for Physical memory space
	for(int i=0; i< memSize; i++)
	{
		Phy_mem[i]='0';
	}

	//for freeFrame list space.
	for(int j=0; j<memSize; j++)
	{
		freeFrameList[j]='v';
	}
}

//when replace = 0, the process will be create FIFO function.
int Fifo_Algorithm(int pid, int page)
{

	int SwapOutF=0;
	struct Fifo *pre=NULL;
	struct Fifo *pre2=NULL;
	struct Fifo *create =NULL;
	struct Fifo *Minimum=NULL;

	pre = head;
	Minimum = pre;

	create = new struct Fifo;

	clock_t time1;
	double limit=100000;
	while((pre->next !=NULL)&&(limit > pre->time))
	{
		if((pre->time)<(Minimum->time))
			Minimum = pre;

		pre=pre->next;
	}
	if((pre->time)<(Minimum->time))
		pre = tail;
	else
		pre = Minimum;


	create->pid = pid;
	create->page = page;
	create->frame = pre->frame;
	//cout << pre->frame <<endl;


	time1=clock();
	create->time = ((float)time1)/CLOCKS_PER_SEC*1000;//milesecond
	create->next = NULL;


	struct processlist *search=NULL;


//	cout << pre->pid<< " " << pre->page<<" "<<pre->frame <<endl;


	search = tracking(pre->pid);
	search->pageTable[pre->page]=-1;

	if(pre == head)
	{
		create->next = head->next;
		head = create;
	
		pre->next=NULL;
	}
	else if(pre == tail)
	{
		pre2=ManageFifo1(pre->pid,pre->page);
		pre2->next=create;
		tail = create;
	}
	else
	{
		pre2=ManageFifo1(pre->pid,pre->page);
	//	pre2->next=create;
		create->next=pre->next;
		pre2->next=create;
		pre->next=NULL;
	}
	SwapOutF=search->pageTable[pre->page];
	search->pageTable[pre->page]=-1;
	
	return SwapOutF;
}




//when replace = 1, the process will be create LRU function.
int LRU()
{

	int SwapOut=0;

	struct processlist *search_L =NULL;

	//search_L -> pageTable = new char[top->next];

	SwapOut = top->frame;

	//tracking pid.
	search_L = tracking(top->pid);
	search_L -> pageTable[top->page] = -1;//-1 becasuse,
	
	//pop the data.
	top=top->next;	


	return SwapOut;
}




struct Fifo *ManageFifo1(int pid, int page)
{
	struct Fifo *search=NULL;
	struct Fifo *pre =NULL;

	search = head;

	while(search->next->next !=NULL)
	{
		if((search->next->page==page)&& (search->next->pid==pid))
		{
		
			return search;
		}

		search=search ->next;
	}
	if((search->next->pid)&&(search->next->page==page))
	{
		return search;
	}
	return NULL;

}



//
int allocate(int allocSize, int pid)
{
	struct processlist *tmp = new struct processlist();
	struct processlist *search=NULL;
	struct processlist *tmp3=NULL;
	struct processlist *tmp2=NULL;
	int counter=0;
	tmp->pid = pid;
	tmp->size = allocSize; //page size.
	tmp->pageTable = new int[allocSize];
	tmp->link = NULL;
	int Random;
	int actualSize=0;
	int state=0;
	int Scounter=0;
	int pointout_exist=0;
	
	for(int i=0; i<Phy_size; i++)
	{
		if(freeFrameList[i]=='v')
		actualSize++;
	}
	
	//check size.//We need to extend FiFO and LRu in here


	//check pid number
	search = front;	
	while(search!=NULL)
	{
		if(search->pid == pid)
		{
		 pointout_exist = 1;//there exist same process
		tmp2 = new struct processlist();
		allocSize=search->size+allocSize;


		Scounter=search->size;


		tmp2->pageTable=new int[allocSize];
		tmp2->pid = pid;
		tmp2->size = allocSize;


		//copy previous value
		int arr[allocSize];
		for(int i=0; i<allocSize; i++)
		tmp2->pageTable[i]=search->pageTable[i];
/*
		


*/
			tmp3 = tmp2;
			if(search == front)
			{
			tmp2->link = front->link;
			front = tmp2;
			}


			else
			{
			//find previous node
			struct processlist *find=front;
			while(find->link != NULL)
			{
				if(find->link->pid == pid)
				{
					break;
				}
					find=find->link;	
			}

				if(search == rear)
				{
					find->link = tmp2;
					rear = tmp2;
				}
				else
				{
					tmp2->link = search->link;
					find->link = tmp2;
				}	
			}
			//terminate function.
//			return -1;
			break;
		}

		else
		search = search->link;
	}

//	cout << "123" <<endl;
	
	int countFreeFrame=0;
	for(int i=0; i<Phy_size; i++)
	{
		if(freeFrameList[i] == 'v')
			countFreeFrame++;
	}
////////////////////////////////////////////////////////
	while(Scounter<allocSize)
	{		

		if(countFreeFrame <= Phy_size && countFreeFrame>0)// there is available frame
		{
			int Random = rand()%10;

			if(freeFrameList[Random]=='v')
			{
				if(pointout_exist == 0)
				{	
					tmp->pageTable[Scounter]=Random;
				}
				else
					tmp2->pageTable[Scounter]=Random;
				Record(pid,Scounter,Random);
				freeFrameList[Random]='i';
				Scounter++;
				countFreeFrame--;
				PF++;			
			}
			else
				continue;
		
		}
		else
		{
			int return_frame=0;

			if(replace == 0)
				return_frame = Fifo_Algorithm(pid,Scounter);
			else
				return_frame =LRU();

			Record(pid,Scounter,return_frame);

			if(pointout_exist == 0)
				tmp->pageTable[Scounter]=return_frame;
			else
				tmp2->pageTable[Scounter]=Random;
			Scounter++;
			PF++;
		}

	
	}
	
	if(pointout_exist == 0)
	{
		if(front==NULL)
		{
			front = tmp;
			rear = tmp;	
		}

		else
		{
			rear->link = tmp;
			rear = tmp;
		}
	}


}




int deallocate(int pid)
{
	struct processlist *Deloc = front;
	struct processlist *search = NULL;

	search=front;
	while(search->link != NULL)
	{
		if(search->pid == pid)
		break;
		search=search->link;
	} 
	if(front->pid ==pid)
		search = front;
	
	while(search->size != 0)
	{	
		freeFrameList[search->pageTable[search->size-1]]='v';
		search->size =search->size - 1;
	}

	// [A]-[]-[]
	if(Deloc->pid == pid)
	{
		front = front->link;

	}
	//[]-[B]-[]   []-[]-[C]
	else
	{
		while(Deloc->link != NULL)
		{
			if(Deloc->link->pid==pid)
			{
				break;
			}
			Deloc = Deloc->link;
		}
		Deloc->link =search->link; 		
	}
}


//
int write(int pid, int logical_address)
{
	struct processlist *search = front;


	while(search->link != NULL)
	{
		if(search->pid == pid)
		{
		int Walmart = search->pageTable[logical_address];
		if(Walmart == -1)
			cout << "Page fault occur!"<<endl;
		else
			Phy_mem[Walmart]='1';
		break;	
		}
	search=search->link;		
	}

	//final pid link check.	
	if(search->pid == pid)
	{
		int Walmart = search->pageTable[logical_address];
		Phy_mem[Walmart]='1';
	}

}



//
int read(int pid, int logical_address)
{
	struct processlist *Rsearch = front;

	while(Rsearch->link != NULL)
	{
		if(Rsearch->pid==pid)
		{
		int Tubu = Rsearch->pageTable[logical_address];
	//	cout << Tubu << " ";
		if(Tubu == -1)
			cout << "Page Fault is occur"<<endl;
		else
			cout <<  "frame : " <<Phy_mem[Tubu]<<endl;
		break;
		}
		Rsearch=Rsearch->link;
	}

	if(rear->pid == pid)
	{
		int Tubu = Rsearch->pageTable[logical_address];
		cout <<  "frame : "<<Phy_mem[Tubu]<<endl;
	}


}


void printMemory(void)
{	
	struct processlist *track;

	//print Physical memory space.
	cout<<"Physical memory space"<<endl;
	for(int i=0; i<Phy_size; i++)
	{
	cout << Phy_mem[i]<<" ";
	}
	cout<<endl<<"Free frame list"<<endl;
	//print valid of Freeframelist
	for(int j=0; j<Phy_size; j++)
	{
		if(freeFrameList[j]=='v')
		{
			cout << j << " ";
		}
	}


	cout<<endl;
	//print process
	if(front==NULL)
	{
		cout<<"Nothing in process"<<endl;
	}
	else
	{
		track=front;
		for(track; track->link!=NULL; track=track->link)
		{		
		cout <<"# process    : "<< track->pid  <<endl;
		cout <<"size process : "<< track->size <<endl;
		}
	cout<<"# process    : "<< track->pid<<endl;
	cout<<"size process : "<< track->size<<endl; 
	}



}


void PageFault_print()
{
	cout <<"Page Fualt : "<<  PF<<endl;
}



int main()
{

	while(1)
	{	
		cout<<"Please select the option"<<endl;
		cout<<"'F' FIFO algorithm"<<endl;
		cout<<"'L' LRU algorithm"<<endl;
		cout<<"Park >>";

		
		char input2[10];
		char order2;
		char *chop[1];
		cin.getline(input2, 20); 
		char *token2=strtok(input2, " ");
		int j=0;

		order2 = input2[0];



		if(order2 == 'F' )
		{	
			replace=0;
			break;
		}
		else if(order2 =='L')
		{
			replace=1;
			break;	
		}
		else
		{
			cout <<"Please enter again"<<endl;
		}
	
	}

	cout<<"replace : " << replace <<endl;
	cout<<"'M' Memory Manager"<<endl;
	cout<<"'A' Allocation"<<endl;
	cout<<"'D' Deallocation"<<endl;
	cout<<"'R' Read"<<endl;
	cout<<"'W' Write"<<endl;
	cout<<"'P' Print Memory"<<endl;
	cout<<"'O' Print page fault"<<endl;
	cout<<"'Q' Quit"<<endl;
	

	while(1)
	{
		cout << "Park >>";
		int i=0;
		char input[20];
		char order;
		char* chop[4];		
		cin.getline(input,20);
		char *token=strtok(input, " ");
		
	
		order = input[0];
	


		while(token!=NULL)
		{
		chop[i]=token;
		token=strtok(NULL," ");
		i++;	
		}

	int iNum1=atoi(chop[1]);
	int iNum2=atoi(chop[2]);
		


	switch(order)
	{

		
		case 'M':
		memoryManager(iNum1,iNum2);
		break;
		
		case 'A':
		allocate(iNum1,iNum2);
		break;
	
		case 'D':
		deallocate(iNum1);
		break;

		case 'P':
		printMemory();
		break;
		
		case 'R':
		read(iNum1,iNum2);
		break;

		case 'W':
		write(iNum1,iNum2);
		break;

		case 'O':
		PageFault_print();
		break;
			
		case 'Q':
			return -1;
			break;

		default :	
			return -2;
	
	}	
		
	}
		return 0;
}

