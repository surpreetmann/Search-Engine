//#include<string.h>

//#define BASE_URL "www.chitkara.edu.in"
struct LINK
{
char *url;
int link_key;
struct LINK* next;
int link_depth;
int link_flag;
};

struct LINK* head=NULL,*last=NULL;

struct Hash
{
struct LINK* first;
struct LINK* last;
int count;
};

struct Hash hash[50]={NULL};

void initialise()
{
	/*static int c=0;
	if(c)
		return;
	c=1;*/
	int i;
	for(i=0;i<50;i++)
	{
		hash[i].first=NULL;
		hash[i].last=NULL;
		hash[i].count=0;
	}
}


//----------------------------------------------------

int generate_key(char *url)
{
	int sum=0,i=0;
	int l=strlen(url);
	for(i=0;i<l;i++)
	{
		sum+=(int)(url[i]);
			
	}
	int key=sum%50;
return key;
}
//-----------------------------------------------------
void display(struct LINK* h)
{
	struct LINK* p=h;
	
	int i=1;
	while(p!=NULL)
	{
	  printf("\n%d. key= %d link= %s depth= %d is_visited=%d \n",i,p->link_key,p->url,p->link_depth,p->link_flag);
		p=p->next;
		i++;
	}
}

//------------------------------------------------------------------

void save_links_to_file(struct LINK* head)
{
	FILE *ptr=fopen("/home/surpreet/Desktop/searchengine/links.txt","w");
	
	struct LINK *temp=head;
	while(temp!=NULL)
	{
		fprintf(ptr,"%s ",temp->url);
		fprintf(ptr,"%d ",temp->link_key);
		fprintf(ptr,"%d ",temp->link_depth);
		fprintf(ptr,"%d\n",temp->link_flag);
		temp=temp->next;
		
	}
}
	
//------------------------------------------------------------------





void retrieve_links_from_file()
{
		
	initialise();	
	head=NULL;
	//struct LINK* temp,*pre=NULL,*p;
	FILE *ptr=fopen("/home/surpreet/Desktop/searchengine/links.txt","r");
	char ch;
	int i=0,j=0,k=0;
	char ln[150]={0};
	char url[125]={0};   //---same as that in struct---//
	char *urls=NULL;
	struct LINK *p;
	
	while((ch=fgetc(ptr))!=EOF)
	{
		if(ch=='\n')
		{
			j=0;
			k=0;
			i=0;
			ln[i]='\0';
			struct LINK *temp=(struct LINK*)calloc(1,sizeof(struct LINK));
			
			while(ln[k]!=' ')
			{
				url[j]=ln[k];
				j++;
				k++;
			}
			url[j]='\0';
			urls=(char*)calloc(1,100);
			strcpy(urls,url);
			
			temp->link_key=generate_key(url);
			int key=temp->link_key;
			temp->url=urls;
			j=0;
			
			k++;
			while(ln[k]!=' ')
			{
				k++;
			}
			k++;
			while(ln[k]!=' ')
			{
				
				url[j]=ln[k];	
				k++;
				j++;	
	
			}
			url[j]='\0';
			temp->link_depth=atoi(url);
			
			j=0;
			k++;
			while(ln[k]!='\0')
			{
				
				url[j]=ln[k];
				k++;
				j++;
			}
			url[j]='\0';
			temp->link_flag=atoi(url);
			//j=0;
			//k=0;
			if(head==NULL)	
			{
				temp->next=NULL;
				head=temp;
				last=temp;
				hash[key].first=temp;
				hash[key].last=temp;
                        	hash[key].count=1;
                        	continue;
			}
			else
			{
				if(hash[key].first==NULL)
				{
					last->next=temp;
					temp->next=NULL;
					
					last=temp;
                        	       hash[key].first=temp;
                        	       hash[key].last=temp;
                        	       hash[key].count=1;
				}
				else
				{
					p=hash[key].first;
					 if((strcmp(hash[key].first->url,temp->url)==0)||(strcmp(hash[key].last->url,temp->url)==0))
                                        {
                                                free(temp);
                                                free(urls);
                                                continue;
                                        }
					int f=0;
					while(p->next!=NULL && p->next->link_key==key)
					{
						if(strcmp(p->url,temp->url)==0)
						{
							free(temp);
							f=1;
							break;
						}
						p=p->next;
					}
					if(f)
					continue;
					if(p->next==NULL)
						last=temp;
					temp->next=p->next;
					p->next=temp;
					 hash[key].last=temp;
                        	        hash[key].count++;
				}
			}
			ln[0]='\0';		
		}
		else
		{
			ln[i++]=ch;	
		}
	}
	display(head);
}
 

void put_links_in_list(char **result,int l,int depth)
{	
		
	int i;
	struct LINK* temp,*p;
	for(i=0;i<l;i++)
	{
		temp=(struct LINK*)calloc(1,sizeof(struct LINK));
		temp->link_key=generate_key(result[i]);
		int key=temp->link_key;
		temp->url=result[i];
		temp->link_depth=depth+1;
		if(head==NULL)	
		{
			temp->next=NULL;			
			temp->link_depth=depth;
			temp->link_flag=1;
			head=temp;
			last=temp;
			hash[key].first=temp;
			hash[key].last=temp;
                        hash[key].count=1;
			
                        continue;
		}
		else
		{
			if(hash[key].first==NULL)
			{
				last->next=temp;
				temp->next=NULL;
				temp->link_flag=0;
				last=temp;
                               hash[key].first=temp;
                               hash[key].last=temp;
                               hash[key].count=1;
			}
			else
			{
				p=hash[key].first;
				 if((strcmp(hash[key].first->url,temp->url)==0)||(strcmp(hash[key].last->url,temp->url)==0))
                                        {
                                                free(temp);
                                                
                                                continue;
                                        }				
				int f=0;
				
				while(p->next!=NULL && p->next->link_key==key)
				{
					if(strcmp(p->url,temp->url)==0)
					{
						free(temp);
						f=1;
						break;
					}
					p=p->next;
				}
				if(f)
				continue;
				if(p->next==NULL)
					last=temp;
				temp->link_flag=0;
				temp->next=p->next;
				p->next=temp;
				 hash[key].last=temp;
                                hash[key].count++;
			}
		}
		
	}
		
	save_links_to_file(head); 
display(head);
}
 
//--------------------------------------------------------------

char* fetch_nextURL(int depth)
{
	struct LINK *temp=head;
	while(temp!=NULL)
	{
		if(temp->link_flag==0 && temp->link_depth==depth)
		{
			temp->link_flag=1;
			return temp->url;
		}
		temp=temp->next;
	}
	return NULL;
}
