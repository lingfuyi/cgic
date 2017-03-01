#include <stdio.h>
#include "cgic.h"
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <unistd.h>  
#include <linux/reboot.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SOFILENUM 	10
#define SOFILELEN	20

char Ip[20]={0};
char Port[20]={0};
char HardVersion[20]={0};
char SoftVersion[20]={0};
char IMEI[20]={0};
char CPUID[30]={0};
char ControllerID[30]={0};
char ControllerType[30]={0};
char MAC[30]={0};
char Baudrate232[20]={0};
char Baudrate485[20]={0};
char Lib232[30]={0};
char Lib485[30]={0};
char PortSelect[30]={0};
char TotleSoFile[50][40];
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
const char *ControllerTypeTable[]=
{
	"1",
	"11",
	"22",
	"31",
	"34"
};
char *PortSelectTable[]=
{
	"1",
	"2"
};

static void CopyIndexStr(char *dest , char *src)
{
	int i=0;
	for(i=0 ; i<30 ; i++)
	{
		if('='!=src[i])
		{
			dest[i]=src[i];
		}
		else
		{
			break;
		}
	}
	dest[i]='\0';
}
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
		while(*(dat+1)&&*dat)
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
	stream = popen( "cat /etc/cpuid", "r" ); 
	fread( CPUID, sizeof(char), sizeof(CPUID), stream); 
	pclose( stream );  
	CPUID[sizeof(CPUID)] = '\0';
	stream = popen( "cat /etc/mac", "r" ); 
	fread( MAC, sizeof(char), sizeof(MAC), stream); 
	pclose( stream );  
	MAC[sizeof(MAC)] = '\0';
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
	char StrLine[200];  
	char ptr[30];
	int i=0;
	if((fd = fopen("/data/Tanda.conf","r")) == NULL) 
	{ 
		printf("cant't open file\n");
		fclose(fd);   
		return; 
	} 
	
	while (!feof(fd)) 
	{     
		fgets(StrLine,200,fd);  
		if(StrLine[0]=='#'||StrLine[0]==' ')
		{
			continue;
		}		
		else
		{
			i=DatPos(StrLine , 0);
			memset(ptr , 0 , sizeof(ptr));
			CopyIndexStr(ptr,StrLine);
			if(!strcmp(ptr , "HardVersion"))
			{
				strncpy(HardVersion,&StrLine[i+1],sizeof(HardVersion));
				HardVersion[sizeof(HardVersion)-1]='\0';
			}			
			else if(!strcmp(ptr , "SoftVersion"))
			{
				strncpy(SoftVersion,&StrLine[i+1],sizeof(SoftVersion));
				SoftVersion[sizeof(SoftVersion)-1]='\0';
			}
			else if(!strcmp(ptr , "ControllerType"))
			{
				strncpy(ControllerType,&StrLine[i+1],sizeof(ControllerType));
				ControllerType[sizeof(ControllerType)-1]='\0';
			}
			else if(!strcmp(ptr , "ControllerID"))
			{
				strncpy(ControllerID,&StrLine[i+1],sizeof(ControllerID));
				ControllerID[sizeof(ControllerID)-1]='\0';
			}
			else if(!strcmp(ptr , "IMEI"))
			{
				strncpy(IMEI,&StrLine[i+1],sizeof(IMEI));
				IMEI[sizeof(IMEI)-1]='\0';
			}
			else if(!strcmp(ptr , "ip"))
			{
				strncpy(Ip,&StrLine[i+1],sizeof(Ip));
				Ip[sizeof(Ip)-1]='\0';
			}
			else if(!strcmp(ptr , "port"))
			{
				strncpy(Port,&StrLine[i+1],sizeof(Port));
				Port[sizeof(Port)-1]='\0';
			}
			else if(!strcmp(ptr , "portenable"))
			{
				strncpy(PortSelect,&StrLine[i+1],sizeof(PortSelect));
				PortSelect[sizeof(PortSelect)-1]='\0';
			}
			else if(!strcmp(ptr , "baudrate232"))
			{
				strncpy(Baudrate232,&StrLine[i+1],sizeof(Baudrate232));
				Baudrate232[sizeof(Baudrate232)-1]='\0';
			}			
			else if(!strcmp(ptr , "baudrate485"))
			{
				strncpy(Baudrate485,&StrLine[i+1],sizeof(Baudrate485));
				Baudrate485[sizeof(Baudrate485)-1]='\0';
			}
			else if(!strcmp(ptr , "protocolName232"))
			{
				strncpy(Lib232,&StrLine[i+3],sizeof(Lib232));
				Lib232[sizeof(Lib232)-1]='\0';
			}
			else if(!strcmp(ptr , "protocolName485"))
			{
				strncpy(Lib485,&StrLine[i+3],sizeof(Lib485));
				Lib485[sizeof(Lib485)-1]='\0';
			}
			else
				;
		}
		
	} 
	
	fclose(fd);                    
	
}

int WriteConfDat(void)
{
	
	FILE *fd;
	int SoChoice232;
	int bauChoice232;
	int SoChoice485;
	int bauChoice485;
	int controllertypechoice;
	int portselect[2];
	int i;
	int result , invalid;
	ReadTandaConf();
	cgiFormSelectSingleNormal("controllertype", ControllerTypeTable, 5, &controllertypechoice, 0);
	cgiFormStringNoNewlines("ip", Ip, sizeof(Ip));
	cgiFormStringNoNewlines("port", Port, sizeof(Port));
	cgiFormStringNoNewlines("controllerid", ControllerID, sizeof(ControllerID));
	result = cgiFormCheckboxMultiple("portcheck", PortSelectTable, 2, portselect, &invalid);
	if (cgiFormNotFound == result) 
	{
		return -1;
	}
	else 
	{	
		result=0;
		for (i=0; i < 2; i++)
		{
			if (portselect[i]) 
			{
				result+=(i+1);
			}
		}
	}
	
	GetSoFileList();
	if(1 == result)
	{
		PortSelect[0]='1';
		cgiFormSelectSingleNormal("bau232", BaudrateTable, 11, &bauChoice232, 0);
		cgiFormSelectSingle("net232", TotleSoFile, TotleSoNum, &SoChoice232, 0);
	}
	else if(2 == result)
	{
		PortSelect[0]='2';
		cgiFormSelectSingleNormal("bau485", BaudrateTable, 11, &bauChoice485, 0);
		cgiFormSelectSingle("net485", TotleSoFile, TotleSoNum, &SoChoice485, 0);
	}
	else if(3 == result)
	{
		PortSelect[0]='3';
		cgiFormSelectSingleNormal("bau232", BaudrateTable, 11, &bauChoice232, 0);
		cgiFormSelectSingle("net232", TotleSoFile, TotleSoNum, &SoChoice232, 0);
		cgiFormSelectSingleNormal("bau485", BaudrateTable, 11, &bauChoice485, 0);
		cgiFormSelectSingle("net485", TotleSoFile, TotleSoNum, &SoChoice485, 0);
	}
	else
	{
		return -2;
	}
    system("chmod 777 /data/Tanda.conf");
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
		
		fputs("#ControllerType\nControllerType=",fd);
		fputs(ControllerTypeTable[controllertypechoice], fd);
		fputs("\n\n",fd);

		fputs("#ControllerID\nControllerID=",fd);
		fputs(ControllerID, fd);
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

		fputs("#1 is 232 , 2 is 485 , 3 is all\nportenable=",fd);
		fputc(PortSelect[0], fd);
		fputs("\n\n",fd);
		if(1==result)
		{
			fputs("#Baudrate\nbaudrate232=",fd);
			fputs(BaudrateTable[bauChoice232], fd);
			fputs("\n\n",fd);
			fputs("#protocolName232\nprotocolName232=./",fd);				
			fputs(TotleSoFile[SoChoice232] , fd);
			fputs("\n\n",fd);
			fputs("#Baudrate485\nbaudrate485=",fd);
			fputs(Baudrate485, fd);
			fputs("\n\n",fd);
			fputs("#protocolName\nprotocolName485=./",fd);				
			fputs(Lib485, fd);
			fputs("\n\n",fd);
		}
		else if (2 == result)
		{
			fputs("#Baudrate\nbaudrate232=",fd);
			fputs(Baudrate232, fd);
			fputs("\n\n",fd);
			fputs("#protocolName232\nprotocolName232=./",fd);				
			fputs(Lib232, fd);
			fputs("\n\n",fd);
			fputs("#Baudrate485\nbaudrate485=",fd);
			fputs(BaudrateTable[bauChoice485], fd);
			fputs("\n\n",fd);
			fputs("#protocolName\nprotocolName485=./",fd);				
			fputs(TotleSoFile[SoChoice485], fd);
			fputs("\n\n",fd);
		}
		else
		{
			fputs("#Baudrate\nbaudrate232=",fd);
			fputs(BaudrateTable[bauChoice232], fd);
			fputs("\n\n",fd);
			fputs("#protocolName232\nprotocolName232=./",fd);				
			fputs(TotleSoFile[SoChoice232], fd);
			fputs("\n\n",fd);
			fputs("#Baudrate485\nbaudrate485=",fd);
			fputs(BaudrateTable[bauChoice485], fd);
			fputs("\n\n",fd);
			fputs("#protocolName\nprotocolName485=./",fd);				
			fputs(TotleSoFile[SoChoice485], fd);
			fputs("\n\n",fd);
		}
		fclose(fd);

	}
	else
	{
		printf("err");
		return -3;
	}
	return 0;

}
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
	fprintf(cgiOut, "HardVersion=%s/",HardVersion);
	fprintf(cgiOut, "MAC=%s/",MAC);
	fprintf(cgiOut, "controllertype=%s/",ControllerType);
	fprintf(cgiOut, "controllerid=%s/",ControllerID);
	fprintf(cgiOut, "CPUID=%s/", CPUID);
	fprintf(cgiOut, "IMEI=%s/",IMEI);
	fprintf(cgiOut, "Ip=%s/",Ip);
	fprintf(cgiOut, "Port=%s/",Port);
	if(PortSelect[0]=='1')
	{		
		fprintf(cgiOut, "Portselect=1\n/");
	}
	else if(PortSelect[0]=='2')
	{
		fprintf(cgiOut, "Portselect=2\n/");
	}
	else if(PortSelect[0]=='3')
	{
		fprintf(cgiOut, "Portselect=3\n/");
	}
	fprintf(cgiOut, "bau232=%s/",Baudrate232);
	fprintf(cgiOut, "Lib232=%s/",Lib232);
	fprintf(cgiOut, "bau485=%s/",Baudrate485);
	fprintf(cgiOut, "Lib485=%s/",Lib485);
	GetSoFileList();
	for(i=0 ; i<TotleSoNum ; i++)
	{
		if(strcmp(Lib232 , TotleSoFile[i]))
		{			
			fprintf(cgiOut, "setnet232=%s/",TotleSoFile[i]);
		}
		if(strcmp(Lib485 , TotleSoFile[i]))
		{			
			fprintf(cgiOut, "setnet485=%s/",TotleSoFile[i]);
		}
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
	buf[strlen(buf)]='\0';
	if(strlen(buf)>2)
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
	int ret;
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
		if(!strncmp(UserInput,"getconf" , 7))
		{
			RequistConfig();
		}
		else if(!strncmp(UserInput,"getstatus" , 9))
		{
			GetSysState();
		}
		
	}

	if ((cgiFormSubmitClicked("Setting") == cgiFormSuccess))
	{		
		ret=WriteConfDat();
		if(0==ret)
		{
			SubmitHandle();
		}
		system("echo 6 > /data/uplog");
	}
	return 0;
}





  

