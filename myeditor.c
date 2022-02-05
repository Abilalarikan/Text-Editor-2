#include <ncurses.h>
#include <string.h>
#define SIZE 200


struct node{
	char statement[40]; 
	int next;  
}; 

int PAGE=1;
struct node textbuffer[SIZE];

int free_head;
int inuse_head;
char *FILENAME;
int file_flag=1,delete_flag=1, inUseListSize=0;
int yMax,xMax;

//Additional Functions

int file_size(char *filename){  // function to find the size of a file
	FILE *fp = fopen(filename,"r");
	if(fp == NULL) return 0;
	fseek(fp, 0, SEEK_END);
	int file_size = ftell(fp);
	
	return file_size;
}

int sizeOfInUseList(){ // function to find the size of inuselist
	int a=inuse_head;
	int i=1;
	while(textbuffer[a].next!=-1){
		i++;
		a=textbuffer[a].next;
	}
	return i;
}

void tr(){ // function to trace linkedlists
	int a=inuse_head;
	int b=free_head;
	printw("%d ",inuse_head);
	while(textbuffer[a].next!=-1){
		printw("%d ",textbuffer[a].next);
		a=textbuffer[a].next;
	}
	printw("%d ",textbuffer[a].next);
	printw("\n%d ",free_head);
	while(textbuffer[b].next!=-1){
		printw("%d ",textbuffer[b].next);
		b=textbuffer[b].next;
	}
	printw("%d ",textbuffer[b].next);
}

int getNode(int n){ // function to get nth node
	int a=inuse_head;
	getmaxyx(stdscr,yMax,xMax);
	
	n+=yMax*(PAGE-1);
	
	if(n==0)
		return inuse_head;
	else if(n==-1)
		return -1;
	
	for(int i=0;i<n;i++){
		if(textbuffer[a].next!=-1)
			a=textbuffer[a].next;
		else
			break;
	}	
	return a;
} 

void addToFreeList(int index){ // function to add to freelist
	textbuffer[index].next=free_head;
	free_head=index;
}

void arrange(char ch[40]){ // adds null character at the end of string ch
	int i;
	for( i=0;i<strlen(ch);i++)
		if(ch[i]==10 || ch[i]==13 )
			break;
	ch[i]='\0';
}

void printList(){
	clear();
	struct node temp=textbuffer[inuse_head];
	int i=0,file_flag=1;
	getmaxyx(stdscr,yMax,xMax);
	
	for(;i<(PAGE-1)*yMax;i++)
		temp=textbuffer[temp.next];
	if(PAGE*yMax>=sizeOfInUseList()){
		i=0;
		while(1){
			printw("%s\n",temp.statement);
			i++;
			if(temp.next==-1)
				break;
			temp=textbuffer[temp.next];
		}
	}	
	else{
		for(i=0;i<yMax;i++){
			printw("%s\n",temp.statement);
			temp=textbuffer[temp.next];
		}
	}
	move(i-1,0);
	refresh();
}

//Editing Functions

void edit(char *filename){
	FILE *fp;
	fp=fopen(filename, "r");
	char line[40];
	int i=0;
	inuse_head=0;
	
	if(file_size(filename) != 0){
		while(fgets(line,40,fp)){
			arrange(line);
			strcpy(textbuffer[i].statement,line);
			textbuffer[i].next=i+1;
			i++;
		}
		inUseListSize=i;
	}
	fclose(fp);
	if(file_size(filename) == 0){ // if we are working on an empty file, inuselist must be empty aswell
		textbuffer[inuse_head].next=-1;
		i++;
	}
	else textbuffer[i-1].next=-1;
	if(i<SIZE){
		free_head=i;
		while(i<SIZE){
			strcpy(textbuffer[i].statement,"");
			textbuffer[i].next=i+1;
			if(i==SIZE-1)
				textbuffer[i].next=-1;
			i++;
		}
	}
	else
		free_head=-1;
	
	printList();
}

void insert(char *stat){
	int y_temp,x_temp;
	getyx(stdscr,y_temp,x_temp);
	int a=free_head;
	int NodeIndex=getNode(y_temp);
	
	inUseListSize++; // increment initial line count

	if(inUseListSize==1){
		delete_flag=0;
		a=inuse_head; // because of our edit function freehead points to 1 instead of 0 when operating on an empty file, in order to fix that we need to decrement a
		file_flag=0;
	}
	if(delete_flag){
		free_head=textbuffer[free_head].next; // deleting the node from free list
	}
	delete_flag=1;
	strcpy(textbuffer[a].statement,stat);
	textbuffer[a].next=textbuffer[NodeIndex].next;
	
	if(a==inuse_head) textbuffer[NodeIndex].next=-1;
	else textbuffer[NodeIndex].next=a;

	printList();
	move(y_temp+1,0);
	refresh();
	
}

void delete(){
	int y_temp,x_temp;
	getyx(stdscr,y_temp,x_temp);
	
	int Node=getNode(y_temp);
	int prevNode=getNode(y_temp-1);
	
	inUseListSize--; // decrement initial line count
	
	if(prevNode==-1 && inUseListSize!=0 )
		inuse_head=textbuffer[Node].next;
	else
		textbuffer[prevNode].next=textbuffer[Node].next;
	
	if(inUseListSize != 0){
		addToFreeList(Node);
	}	
	else {
		strcpy(textbuffer[inuse_head].statement, "");
		delete_flag = 0;
	}
	printList();
	move(y_temp,0);
	refresh();	
}

void save(){
	FILE * fp;
   
	fp = fopen (FILENAME,"w");
 	int i=inuse_head;
	while(1){
		fprintf (fp, "%s\n",textbuffer[i].statement);
		i=textbuffer[i].next;
		if(i==-1)
			break;
	}  
	fclose(fp);
}

// main
int main(int argc, char *argv[]){
	FILENAME=argv[1];
	
	initscr();
	raw();
	refresh();
	edit(FILENAME);
	
	int y,x;
	
	while(1){
		getyx(stdscr,y,x);
		noecho();
		char ch=getch();
		
		if(ch=='w'){
			if(PAGE!=1 && y==0){
				clear();
				PAGE--;
				printList();
				move(yMax,0);
				refresh();
			}
			else{
				move(--y,x);
				refresh();
			}
		}
		else if(ch=='z'){
			getmaxyx(stdscr,yMax,xMax);

			if(PAGE*yMax<sizeOfInUseList() && (y+1)==yMax){
				clear();
				PAGE++;
				printList();
				move(0,0);
				refresh();
			}
			else{
				move(++y,x);
				refresh();
			}
		}
		else if(ch=='I'){
			if(sizeOfInUseList() != SIZE){ // don't insert if the file is full
				int y_temp,x_temp;
				getyx(stdscr,y_temp,x_temp);
		
				//getting input from user
				int i=0;
				char text[40]="";
				echo();
				getmaxyx(stdscr,yMax,xMax);
				WINDOW *w=newwin(3,xMax/2,y_temp-1,50);
				refresh();
				box(w,0,0);
				wrefresh(w);
				
				char c=mvgetch(y_temp,51);
				text[i]=c;
				while(1){
					c=getch();
					if(c==10)
						break;
					i++;
					text[i]=c;	
				}
				insert(text);	
			}
		}
		else if(ch=='D'){
				delete();
		}
		else if(ch=='S'){
			save();
		}
		else if(ch=='X'){
			int xMax,yMax;
			clear();
			getmaxyx(stdscr,yMax,xMax);
			
			WINDOW *win = newwin(yMax/4,xMax/4,yMax/4,xMax/4);
			refresh();
			box(win,0,0);
			mvwprintw(win,yMax/8,(xMax/16)+3,"GOOD BYE!!!");
	
			wrefresh(win);
			break;
		}
	}
	refresh();	
	getch();
	endwin();
}