#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>

void main(int argc,char** argv)
{
if(argc!=2)
{
printf("Usage: %s driver_path\n",argv[0]);
return;
}
int fd=open(argv[1],O_RDWR);
int op;
char s[10];


while(1)
{
printf("1)write\n2)read\n3)exit\n");
scanf("%d",&op);

if(op==1)
{
	bzero(s,sizeof(s));
	printf("Enter the key and value: ");
	scanf("%s",s);
	printf("%s\n",s);
	write(fd,s,strlen(s)+1);
}
else if(op==2)
{
	int i;
	bzero(s,sizeof(s));
	printf("Enter the key value: ");
	scanf("%s",s);
	s[strlen(s)]=',';
	i=read(fd,s,sizeof(s));
	printf("%s\ni=%d\n",s,i);
}
else if(op==3)
	break;
}
printf("ads\n");
close(fd);
}
