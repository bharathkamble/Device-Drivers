#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>

void main(int argc,char **argv)
{
if(argc!=2)
{
	printf("Usage: %s device_name\n",argv[1]);
	return;
}

char s[1024];
int fd=open(argv[1],O_RDWR);
int op,i;
while(1)
{
	printf("1)Enter msg\n2)Read msg\n3)Exit\n");
	scanf("%d",&op);
	if(op==1)
	{
		printf("Enter the msg: ");
		scanf("%s",s);
		write(fd,s,strlen(s));
	}
	else if(op==2)
	{
		if(!read(fd,s,1024))
			{
			perror("Read:");
			return;
			}
		printf("%s\n",s);
	}
	else if(op==3)
		break;

}
close(fd);
}
