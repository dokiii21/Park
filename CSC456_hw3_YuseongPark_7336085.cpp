#include<iostream>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cstdlib>
#include<ctime>

using namespace std;

struct processlist
{
	int pid;
	int size;
	int *pageTable;
	struct processlist *link;
};


int Phy_size;
char *Phy_mem;
char *freeFrameList;
struct processlist *front=NULL;
struct processlist *rear=NULL;


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

int allocate(int allocSize, int pid)
{
	
	struct processlist *tmp = new struct processlist();
	struct processlist *search=NULL;

	int counter=0;
	tmp->pid = pid;
	tmp->size = allocSize; //page size.
	tmp->pageTable = new int[allocSize];
	tmp->link = NULL;
	int Random;
	int j=0;
	int state=0;
	

	for(int i=0; i<Phy_size; i++)
	{
		if(freeFrameList[i]=='v')
		j++;
	}
	
	//check size.
	if(allocSize >j)
	{
		cout<<"The allocation size already overflow"<<endl;
		return -1; 
	}

	//check pid number
	search = front;
	while(search!=NULL)
	{
		if(search->pid == pid)
		{
		struct processlist *tmp2 = new struct processlist();
		allocSize=search->size+allocSize;
		int Scounter=search->size;
		tmp2->pageTable=new int[allocSize];
		tmp2->pid = pid;
		tmp2->size = allocSize;
//		cout<<"Size"<<tmp2->size <<endl;

		//copy previous value
		int arr[allocSize];
		for(int i=0; i<allocSize; i++)
		tmp2->pageTable[i]=search->pageTable[i];


		while(Scounter < allocSize)
		{
			int Random = rand()%10;
			if(freeFrameList[Random]=='v')
			{
				tmp2->pageTable[Scounter]=Random;
				freeFrameList[Random]='i';
				tmp2->link=NULL;
				Scounter++;
				break;
			}
	
		}
		
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
			return -1;
			break;
		}
		else
			search = search->link;
	}

	while(counter<allocSize)
	{		
		int Random = rand()%10;
		if(freeFrameList[Random]=='v')
		{
			tmp->pageTable[counter]=Random;
			freeFrameList[Random]='i';
			counter++;
		}
//		cout << "allocate : " << Random <<endl;	
	}
	//for(int i=0; i<tmp->size; i++)
	//	cout << tmp->pageTable[i]<<" " <<endl;

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
	//cout << search->pageTable[search->size-1] <<endl;
	//cout << freeFrameList[search->pageTable[search->size-1]]<<endl;
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

int write(int pid, int logical_address)
{
	struct processlist *search = front;


	while(search->link != NULL)
	{
		//cout << "asddfad"<< search->pid <<endl;
		if(search->pid == pid)
		{
		int Walmart = search->pageTable[logical_address];
	//	cout << Walmart <<" ";
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


int main()
{
		cout<<"Please select the option"<<endl;
		cout<<"'M' Memory Manager"<<endl;
		cout<<"'A' Allocation"<<endl;
		cout<<"'D' Deallocation"<<endl;
		cout<<"'R' Read"<<endl;
		cout<<"'W' Write"<<endl;
		cout<<"'P' Print Memory"<<endl;
		cout<<"'Q' Quit"<<endl;
		
	while(1)
	{
		cout << "Park >>";
		int i=0;
		char input[20];
		char order;
		char* chop[3];		
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
		
		case 'Q':
			return -1;
			break;

		default :	
			return -2;
	
	}	
		
	}
		return 0;
}

