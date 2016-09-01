#include <stdio.h>
#include "cgic.h"
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <unistd.h>  
#include<linux/reboot.h>

#define SOFILENUM 	10
#define SOFILELEN	20

char Ip[20]={0};
char Port[20]={0};
char Rs232[20]={0};
char Rs485[20]={0};
char HardVersion[20]={0};
char SoftVersion[20]={0};
char IMEI[20]={0};
char MAC[30]={0};
char Baudrate[20]={0};
char Lib[20]={0};
char Porttype[20]={0};
char TotleSoFile[10][20];
char TotleSoNum=0;
const char *BaudrateTable[] = 
{
	"2400",
	"4800",
	"9600",
	"14400",
	"19200",
	"38400",
	"57600",
	"115200",
	"230400",
	"460800",
	"921600"
};

char *porttable[]=
{
	"1",
	"2"
};

enum UpStatus
{
	UPSUCCESS,		//状态0 上传成功
};

/*
Find the '=' pos and get the config dat
*/
static int DatPos(char *dat , int index )
{
	int i=0;
	if(index==0)//find the config file
	{
		while((*dat++)!='=')
		{
			i++;
		}
		return i;
	}
	else //find the *.so file
	{
		while(*dat&&*(dat+1))
		{
			if(*dat=='s'&&*(dat+1)=='o')
			{
				return 1;
			}
			dat++;
		}
		return 0;
	}
}

/*Find How many So file */
void GetFileOfSo(void)
{
	FILE   *stream;  
	FILE    *wstream;
	char   buf[1024]; 
	memset( buf,0,sizeof(buf));
	stream = popen( " ls /data/*.so", "r" ); 
	wstream = fopen( "/data/Numberso", "w+"); 
	system("chmod 777 /data/Numberso");
	fread( buf, sizeof(char), sizeof(buf),  stream); 
	fwrite( buf, 1, sizeof(buf), wstream );
	pclose( stream );  
	fclose( wstream );
}


void GetMAC(void)
{
	FILE   *stream;  
	stream = popen( "cat /etc/mac", "r" ); 
	fread( MAC, sizeof(char), sizeof(MAC),	stream); 
	pclose( stream );  
	MAC[sizeof(MAC)]='\0';
}


void GetSoFileList(void)
{
	
	FILE *fd;
	TotleSoNum=0;
	char StrLine[1024];   
	int i;
	if((fd = fopen("/data/Numberso","r")) == NULL) 
	{ 
		return; 
	} 
	while (!feof(fd)) 
	{     
		fgets(StrLine,1024,fd); 
		i=DatPos(StrLine,1);
		if(i==1) //find a so file
		{			
			strcpy(TotleSoFile[TotleSoNum++], &StrLine[6]);
		}
		else
		{
			break;
		}
	}	
	fclose(fd); 
}

/*When open html we should read configfile to fix it*/
void ReadTandaConf(void)
{
	FILE *fd;
	char StrLine[1024];  
	char ptr[20];
	int i=0;
	if((fd = fopen("/data/Tanda.conf","r")) == NULL) 
	{ 
		return; 
	} 

	while (!feof(fd)) 
	{     
		fgets(StrLine,1024,fd);  
		if(StrLine[0]=='#'||StrLine[0]==' ')
		{
			continue;
		}
		else
		{
			i=DatPos(StrLine , 0);
			memset(ptr , 0 , sizeof(ptr));
			strncpy(ptr , StrLine , 3);
			ptr[3]='\0';
			if(!strcmp(ptr , "Har"))
			{
				strncpy(HardVersion,&StrLine[i+1],sizeof(HardVersion));
				HardVersion[sizeof(HardVersion)-1]='\0';
				continue;
			}			
			if(!strcmp(ptr , "Sof"))
			{
				strncpy(SoftVersion,&StrLine[i+1],sizeof(SoftVersion));
				SoftVersion[sizeof(SoftVersion)-1]='\0';
				continue;
			}

			if(!strcmp(ptr , "IME"))
			{
				strncpy(IMEI,&StrLine[i+1],sizeof(IMEI));
				IMEI[sizeof(IMEI)-1]='\0';
				continue;
			}
			if(!strcmp(ptr , "ip="))
			{
				strncpy(Ip,&StrLine[i+1],sizeof(Ip));
				Ip[sizeof(Ip)-1]='\0';
				continue;
			}
			if(!strcmp(ptr , "por")&&StrLine[4]=='=')
			{
				strncpy(Port,&StrLine[i+1],sizeof(Port));
				Port[sizeof(Port)-1]='\0';
				continue;
			}
			if(!strcmp(ptr , "rs2"))
			{
				strncpy(Rs232,&StrLine[i+1],sizeof(Rs232));
				Rs232[sizeof(Rs232)-1]='\0';
				continue;
			}
			if(!strcmp(ptr , "rs4"))
			{
				strncpy(Rs485,&StrLine[i+1],sizeof(Rs485));
				Rs485[sizeof(Rs485)-1]='\0';
				continue;
			}
			if(!strcmp(ptr , "bau"))
			{
				strncpy(Baudrate,&StrLine[i+1],sizeof(Baudrate));
				Baudrate[sizeof(Baudrate)-1]='\0';
				continue;
			}
			if(!strcmp(ptr , "pro"))
			{
				strncpy(Lib,&StrLine[i+3],sizeof(Lib));
				Lib[sizeof(Lib)-1]='\0';
				continue;
			}
			if(!strcmp(ptr , "por")&&StrLine[4]=='t')//porttype
			{
				strncpy(Porttype,&StrLine[i+1],sizeof(Porttype));
				Porttype[sizeof(Porttype)-1]='\0';
				continue;
			}
		}
		
	} 
	fclose(fd);                    
	
}

void WriteConfDat(void)
{
	
	FILE *fd;
	int SoChoice;
	int bauChoice;
	int portchoice;
	int i;
	ReadTandaConf();
	cgiFormStringNoNewlines("ip", Ip, sizeof(Ip));
	cgiFormStringNoNewlines("port", Port, sizeof(Port));
	cgiFormRadio("porttype", porttable, 2, &portchoice, 0);
	cgiFormSelectSingleNormal("bau", BaudrateTable, 11, &bauChoice, 0);
	//printf("<p>bau=%s ,%d<p>\n",BaudrateTable[bauChoice],bauChoice);
	GetSoFileList();
	cgiFormSelectSingle("net", TotleSoFile, TotleSoNum, &SoChoice, 0);
	fd=fopen("/data/Tanda.conf" ,"w");
	if(fd)
	{			
		fputs("###################################\n",fd);
		fputs("#    Tanda Adapter Config File    #\n",fd);
		fputs("###################################\n\n",fd);
		
		fputs("#HardVersion\nHardVersion=",fd);
		fputs(HardVersion, fd);
		fputs("\n\n",fd);

		
		fputs("#SoftVersion\nSoftVersion=",fd);
		fputs(SoftVersion, fd);
		fputs("\n\n",fd);

		fputs("#IMEI\nIMEI=",fd);
		fputs(IMEI, fd);
		fputs("\n\n",fd);

		fputs("Service IP\nip=",fd);
		fputs(Ip , fd);
		fputs("\n\n",fd);

		fputs("#Service Port\nport=",fd);
		fputs(Port , fd);
		fputs("\n\n",fd);

		fputs("#rs232port=/dev/ttymxc\nrs232port=",fd);
		fputs(Rs232, fd);
		fputs("\n\n",fd);

		fputs("#rs485port=/dev/ttymxc\nrs485port=",fd);
		fputs(Rs485, fd);
		fputs("\n\n",fd);

		fputs("#Baudrate\nbaudrate=",fd);
		if(bauChoice==0)
			fputs(Baudrate, fd);
		else
			fputs(BaudrateTable[bauChoice], fd);
		fputs("\n\n",fd);
		
		fputs("#protocolName\nprotocolName=./",fd);				
		fputs(TotleSoFile[SoChoice] , fd);
		fputs("\n\n",fd);

		fputs("#Connect Port Select\nporttype=",fd);
		fputs(porttable[portchoice], fd);
		fputs("\n\n",fd);
		fclose(fd);

	}
	else
	{
		printf("err");
	}

}

/*涓婅浇閫氳鏂囦欢*/
void UpLoadsoFile(void)
{
	cgiFilePtr file;
	FILE *fd;
	char name[1024];
	char path[50];
	char contentType[1024];
	int size;
	unsigned int got;
	char *tmp = NULL;
	if (cgiFormFileName("sofile", name, sizeof(name)) != cgiFormSuccess) {
		return ;
	} 
	cgiFormFileSize("sofile", &size);
	cgiFormFileContentType("sofile", contentType, sizeof(contentType));
	if (cgiFormFileOpen("sofile", &file) != cgiFormSuccess) {
		return ;
	}
	/*write file */
	
	tmp=(char *)malloc(sizeof(char)*size);
	strcpy(path , "/data/");
	strcat(path, name);  
	fd=fopen(path ,"w+");
	if(fd==NULL)
	{
		return ;
	}
	while (cgiFormFileRead(file, tmp, size, &got) ==
		cgiFormSuccess)
	{
		fwrite(tmp, size, sizeof(char), fd);
	}
	cgiFormFileClose(file);
	free(tmp);
	fclose(fd);
	return ;
}


void UpLoadUpdateFile(void)
{
	cgiFilePtr file;
	FILE *fd;
	char name[1024];
	char path[50];
	char contentType[1024];
	int size;
	unsigned int got;
	char *tmp = NULL;
	if (cgiFormFileName("updatefile", name, sizeof(name)) != cgiFormSuccess) {
		return ;
	} 
	cgiFormFileSize("updatefile", &size);
	cgiFormFileContentType("updatefile", contentType, sizeof(contentType));
	if (cgiFormFileOpen("updatefile", &file) != cgiFormSuccess) {
		return ;
	}
	/*write file */
	
	tmp=(char *)malloc(sizeof(char)*size);
	strcpy(path , "/data/update/");
	strcat(path, name);  
	fd=fopen(path ,"w+");
	if(fd==NULL)
	{
		return ;
	}
	while (cgiFormFileRead(file, tmp, size, &got) ==
		cgiFormSuccess)
	{
		fwrite(tmp, size, sizeof(char), fd);
	}
	cgiFormFileClose(file);
	free(tmp);
	fclose(fd);
	return ;
}

void UpLoadMD5File(void)
{
	cgiFilePtr file;
	FILE *fd;
	char name[1024];
	char path[50];
	char contentType[1024];
	int size;
	unsigned int got;
	char *tmp = NULL;
	if (cgiFormFileName("MD5file", name, sizeof(name)) != cgiFormSuccess) {
		return ;
	} 
	//fprintf(cgiOut, "The filename submitted was: ");
	//cgiHtmlEscape(name);
	//fprintf(cgiOut, "<p>\n");
	cgiFormFileSize("MD5file", &size);
	//fprintf(cgiOut, "The file size was: %d bytes<p>\n", size);
	cgiFormFileContentType("MD5file", contentType, sizeof(contentType));
	//fprintf(cgiOut, "The alleged content type of the file was: ");
	//cgiHtmlEscape(contentType);
	//fprintf(cgiOut, "<p>\n");

	
	//fprintf(cgiOut, "Of course, this is only the claim the browser made when uploading the file. Much like the filename, it cannot be trusted.<p>\n");
	//fprintf(cgiOut, "The file's contents are shown here:<p>\n");
	if (cgiFormFileOpen("MD5file", &file) != cgiFormSuccess) {
		return ;
	}
	/*write file */
	
	tmp=(char *)malloc(sizeof(char)*size);
	strcpy(path , "/data/update/");
	strcat(path, name);  
	fd=fopen(path ,"w+");
	if(fd==NULL)
	{
		return ;
	}
	//fprintf(cgiOut, "<pre>\n");
	while (cgiFormFileRead(file, tmp, size, &got) ==
		cgiFormSuccess)
	{
		fwrite(tmp, size, sizeof(char), fd);
		//cgiHtmlEscapeData(tmp, size);
	}
	//fprintf(cgiOut, "</pre>\n");
	cgiFormFileClose(file);
	free(tmp);
	fclose(fd);
	return ;
}

void SubmitHandle(void)
{
	system("chmod 777 /data");
	system("chmod 777 /data/update");
	UpLoadsoFile();	
	UpLoadUpdateFile();
	UpLoadMD5File();
}
void RequistConfig(void)
{
	FILE *fd;
	char StrLine[1024];
	int i=0;	
	GetFileOfSo();
	GetMAC();
	ReadTandaConf();
	fprintf(cgiOut, "SoftVersion=%s/",SoftVersion);
	fprintf(cgiOut, "/HardVersion=%s/",HardVersion);
	fprintf(cgiOut, "MAC=%s\n",MAC);
	fprintf(cgiOut, "IMEI=%s/",IMEI);
	fprintf(cgiOut, "Ip=%s/",Ip);
	fprintf(cgiOut, "Port=%s/",Port);
	if(Porttype[0]=='1')
	{
		fprintf(cgiOut, "Porttypea=%s/",Porttype);
	}
	else
	{
		fprintf(cgiOut, "Porttypeb=%s/",Porttype);
	}
	fprintf(cgiOut, "bau0=%s/",Baudrate);
	fprintf(cgiOut, "Lib=%s/",Lib);
	GetSoFileList();
	for(i=0 ; i<TotleSoNum ; i++)
	{
		if(!strcmp(Lib , TotleSoFile[i]))
		{
			continue;
		}
		fprintf(cgiOut, "setnet=%s/",TotleSoFile[i]);
	}
	fprintf(cgiOut, "readend");

}

char GetSysUpState(void)
{
	FILE *fd;
	char c=5;
	if((fd = fopen("/data/uplog","r")) == NULL) 
	{ 
		fprintf(cgiOut,"Can get the lognews");
		return; 
	} 
	c=fgetc(fd); 
	fclose(fd); 
	return c;
}


void GetSysState(void)
{
	FILE   *stream;  
	char   buf[50]; 
	char c;
	memset( buf,0,sizeof(buf));
	stream = popen( "ps | grep SteamSensor | grep -v grep | awk '{print $1}'", "r" ); 
	chmod(stream ,777);
	fread( buf, sizeof(char), sizeof(buf),  stream); 
	buf[sizeof(buf)]='\0';
	if(strlen(buf)>4)	//返回的PID为进程号
	{
		c=GetSysUpState();
		printf("%c=Run",c);
	}
	else
	{
		c=GetSysUpState();
		printf("%c=Stop",c);
	}
	pclose( stream );  
}
int cgiMain() 
{
	char *UserInput = NULL; 		// index the user input data.
   	char *request_method = NULL; 	//index the html transfer type.
	int data_len = 0;		// index the input data's length.
	int lock=0;			
	printf("Content-type:text/html;charset=utf-8\n\n");
	if( ! getenv("REQUEST_METHOD") )
	{
		return -1;
	}
	request_method = getenv("REQUEST_METHOD"); // trans-type.

	 // trans-type : GET 
	if( ! strcmp( request_method, "GET" ) )
	{
		if( getenv( "QUERY_STRING" ) )
		{
			UserInput = getenv( "QUERY_STRING" );
		}
		data_len = strlen( UserInput ); 

		if( NULL == UserInput || 0 == data_len )
		{
			printf( "There's no input data !\n" );
            return -1;
		}
		UserInput[data_len] = '\0';
		if(!strcmp(UserInput,"getconf"))//客户端申请获取数据
		{
			RequistConfig();
		}
		else if(!strcmp(UserInput,"getstatus"))
		{
			GetSysState();
		}
		
	}
	if ((cgiFormSubmitClicked("Setting") == cgiFormSuccess))
	{
			SubmitHandle();
			WriteConfDat();
	}
	return 0;
}





  

