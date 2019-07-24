#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "crawlerlinklist.h"
#define BASE_URL "www.chitkara.edu.in"
#define DEPTH 5

void check_valid(char *argv1)
{
		//wget() for validating URL --without proxy--
		char url[400]="wget --spider ";
		strcat(url,argv1);

		if(!system(url)) //check if url exists or not
			printf("\nValid URL");
		else
		{
			printf("\nInvalid URL");
			exit(1);
		}	
}

void compare_url(char argv1[])
{
	int string_length=strlen(argv1);
	int i=0;    
	char SEED_URL[strlen(argv1)+5];
	    while(argv1[i]!='/' && argv1[i]!='\0')
	    {
	        SEED_URL[i]=argv1[i];
		i++;
	   }
	SEED_URL[i]='\0';

if(strcmp(SEED_URL,BASE_URL)==0)
{
	printf("\nCorrect base url");
}

else
{
	printf("\nIncorrect base url");
	exit(0);
}
}

void check_depth(char argv2[])
{
		int i=atoi(argv2);
		if(i<=DEPTH && i>=1)
		{
				printf("\nCorrect depth");
		}

		else
		{
			printf("\nIncorrect depth");
			exit(0);
		}
}	

void check_dir(char *dir)
{
  struct stat statbuf;
  if ( stat(dir, &statbuf) == -1 ) 
  {
    printf("-----------------\n");
    printf("Invalid directory\n");
    printf("-----------------\n");
    exit(1);
  }
  //Both check if there's a directory and if it's writable
  if ( !S_ISDIR(statbuf.st_mode) ) 
  {
    printf("-----------------------------------------------------\n");
    printf("Invalid directory entry. Your input isn't a directory\n");
    printf("-----------------------------------------------------\n");
    exit(1);
  }
  if ( (statbuf.st_mode & S_IWUSR) != S_IWUSR ) 
  {
    printf("------------------------------------------\n");
    printf("Invalid directory entry. It isn't writable\n");
    printf("------------------------------------------\n");
    exit(1);
  }
}

int NormalizeURL(char* URL)
{
  int len = strlen(URL);
  if (len <= 1 )
    return 0;
  //! Normalize all URLs.
  if (URL[len - 1] == '/')
  {
    URL[len - 1] = 0;
    len--;
  }
  int i, j;
  len = strlen(URL);
  //! Safe check.
  if (len < 2)
    return 0;
  //! Locate the URL's suffix.
  for (i = len - 1; i >= 0; i--)
    if (URL[i] =='.')
      break;
  for (j = len - 1; j >= 0; j--)
    if (URL[j] =='/')
      break;
  //! We ignore other file types.
  //! So if a URL link is to a file that are not in the file type of the following
  //! one of four, then we will discard this URL, and it will not be in the URL list.
  if ((j >= 7) && (i > j) && ((i + 2) < len))
  {
    if ((!strncmp((URL + i), ".htm", 4))
        ||(!strncmp((URL + i), ".HTM", 4))
        ||(!strncmp((URL + i), ".php", 4))
        ||(!strncmp((URL + i), ".jsp", 4))
        )
    {
      len = len; // do nothing.
    }
    else
    {
      return 0; // bad type
    }
  }
  return 1;
}

void removeWhiteSpace(char* html) 
{
  int i;
  char *buffer = calloc(1,strlen(html)+1), *p=calloc (1,sizeof(char)+1);
  memset(buffer,0,strlen(html)+1);
  for (i=0;html[i];i++) 
  {
    if(html[i]>32)
    {
      sprintf(p,"%c",html[i]);
      strcat(buffer,p);
    }
  }
  strcpy(html,buffer);
  free(buffer); free(p);
}

int GetNextURL(char* html, char* urlofthispage, char* result, int pos) 
{
  char c;
  int len, i, j;
  char* p1;  //!< pointer pointed to the start of a new-founded URL.
  char* p2;  //!< pointer pointed to the end of a new-founded URL.

  // NEW
  // Clean up \n chars
  if(pos == 0) {
    removeWhiteSpace(html);
  }
  // /NEW

  // Find the <a> <A> HTML tag.
  while (0 != (c = html[pos])) 
  {
    if ((c=='<') &&
        ((html[pos+1] == 'a') || (html[pos+1] == 'A'))) {
      break;
    }
    pos++;
  }
  //! Find the URL it the HTML tag. They usually look like <a href="www.abc.com">
  //! We try to find the quote mark in order to find the URL inside the quote mark.
  if (c) 
  {  
    // check for equals first... some HTML tags don't have quotes...or use single quotes instead
    p1 = strchr(&(html[pos+1]), '=');
    
    if ((!p1) || (*(p1-1) == 'e') || ((p1 - html - pos) > 10)) 
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*(p1+1) == '\"' || *(p1+1) == '\'')
      p1++;

    p1++;    

    p2 = strpbrk(p1, "\'\">");
    if (!p2) 
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*p1 == '#') 
    { // Why bother returning anything here....recursively keep going...

      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (!strncmp(p1, "mailto:",7))
      return GetNextURL(html, urlofthispage, result, pos+1);
    if (!strncmp(p1, "http", 4) || !strncmp(p1, "HTTP", 4)) 
    {
      //! Nice! The URL we found is in absolute path.
      strncpy(result, p1, (p2-p1));
      return  (int)(p2 - html + 1);
    } else {
      //! We find a URL. HTML is a terrible standard. So there are many ways to present a URL.
      if (p1[0] == '.') {
        //! Some URLs are like <a href="../../../a.txt"> I cannot handle this. 
	// again...probably good to recursively keep going..
	// NEW
        
        return GetNextURL(html,urlofthispage,result,pos+1);
	// /NEW
      }
      if (p1[0] == '/') {
        //! this means the URL is the absolute path
        for (i = 7; i < strlen(urlofthispage); i++)
          if (urlofthispage[i] == '/')
            break;
        strcpy(result, urlofthispage);
        result[i] = 0;
        strncat(result, p1, (p2 - p1));
        return (int)(p2 - html + 1);        
      } else {
        //! the URL is a absolute path.
        len = strlen(urlofthispage);
        for (i = (len - 1); i >= 0; i--)
          if (urlofthispage[i] == '/')
            break;
        for (j = (len - 1); j >= 0; j--)
          if (urlofthispage[j] == '.')
              break;
        if (i == (len -1)) {
          //! urlofthis page is like http://www.abc.com/
            strcpy(result, urlofthispage);
            result[i + 1] = 0;
            strncat(result, p1, p2 - p1);
            return (int)(p2 - html + 1);
        }
        if ((i <= 6)||(i > j)) {
          //! urlofthis page is like http://www.abc.com/~xyz
          //! or http://www.abc.com
          strcpy(result, urlofthispage);
          result[len] = '/';
          strncat(result, p1, p2 - p1);
          return (int)(p2 - html + 1);
        }
        strcpy(result, urlofthispage);
        result[i + 1] = 0;
        strncat(result, p1, p2 - p1);
        return (int)(p2 - html + 1);
      }
    }
  }    
  return -1;
}



int is_already_present(char *temp, char **result, int i)
{
	int j;
	for(j=0;j<i;j++)
	{
		if(strcmp(result[j],temp)==0)
		{
			return 1;
		}
	}
	return 0;
}




void get_urls(char *html_string,int file_size,char *urlofthispage, int depth)
{
	//clearenv();	
	int i=0,j=0,k=0,pos=0;
	char *result_URLBuffer[61];
	char *temp;
	
	
	result_URLBuffer[0]=urlofthispage;
	for(i=1;i<61&&pos<file_size;)
	{
		temp=(char*)calloc(1,125);	
		pos=GetNextURL(html_string, urlofthispage, temp, pos);
		if(temp[strlen(temp)-1]=='/')
			temp[strlen(temp)-1]='\0';
		if(!(is_already_present(temp,result_URLBuffer,i))) 
		{
			result_URLBuffer[i]=temp;
			//printf("%s\n",result_URLBuffer[i]);
			i++;
		}
		else
			free(temp);
	}
	printf("\n---put links in list---\n");
	put_links_in_list(result_URLBuffer,i,depth);
	
	
}


void copy_content_toString(char *open_file,char *url, int depth)
{
	printf("/n---%s----/n",open_file);
	char ch1;
	int i=0;
	struct stat st; //variable which will count length of file.
	stat(open_file,&st);
	int file_size=st.st_size;
	char *html_string=(char*)calloc(1,file_size);
	FILE *fptr=fopen(open_file,"r");
	while((ch1=fgetc(fptr))!=EOF)
	{
		html_string[i++]=ch1;
	}
	html_string[i]='\0';
	printf("\n---get urls called---\n");
	get_urls(html_string,file_size,url,depth);
	free(html_string);
	//printf("%s",html_string);
}

void shift_page(char *url,int depth)
{
	static int count=1;
	char file_name[20]={0},open_file[100]={0};
	char ch[15]={0};
	sprintf(ch,"%d",count);
	
	strcat(file_name,ch);
	strcat(file_name,".txt");
	strcat(open_file,"/home/surpreet/Desktop/searchengine/");
	strcat(open_file,file_name);
	FILE *fptr1= fopen("/home/surpreet/Desktop/searchengine/temp.txt","r");
	FILE *fptr2 = fopen(open_file,"w");
	fprintf(fptr2,"%s\n",url);
	fprintf(fptr2,"%d\n",depth);	
	char p;
		
	while((p=fgetc(fptr1))!=EOF)
	{
		putc(p,fptr2);
	}
	//fclose(fptr2);
	
	count++;
	printf("\n---copy content to string called---\n");
	copy_content_toString(open_file,url,depth);
	fclose(fptr1);
	fclose(fptr2);
}

void get_page(char *url, char *dir,int depth)
{
	char urlbuffer[400]={0}; 
	strcat(urlbuffer, "wget -O ");
	strcat(urlbuffer, dir);
	strcat(urlbuffer,"/temp.txt ");
	strcat(urlbuffer, url);
	system(urlbuffer);
	printf("\n---shift page called---\n");
	shift_page(url,depth);
}

int check_validity(int argc,char *argv[])
{
	if(argc!=4)
        	{
    	    		printf("\nWrong implementation");
            		
        	}        	

		check_valid(argv[1]);
		compare_url(argv[1]);
		check_depth(argv[2]);
		check_dir(argv[3]);
}

int main(int argc,char *argv[])
{
	check_validity(argc,argv);	
	initialise();
	char c;	
	
	printf("Do you want to retreive links from the file: ");
	scanf("%c",&c);
	if(c=='y' || c=='Y')
	{
		retrieve_links_from_file();
	}
	else
	{	
		
		get_page(argv[1],argv[3],1);  ///?????///
	}
	int depth=1;
	char *url=NULL;
	
	while(depth<=atoi(argv[2]))
	{
		
				
		while((url=fetch_nextURL(depth))!=NULL)
		{
			get_page(url,argv[3],depth);
		}
		
			depth++;
		
	}
	display(head);
	
	return 1;        
}
