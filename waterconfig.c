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
#include <sqlite3.h>   
char *ChannelSelectTable[]=
{
	"1",
};
char *TypeSelectTable[]=
{
	"0",
	"1",
	"2",
	"3"
};
struct WaterSystemConfTypedef
{
	unsigned char type[8];
	unsigned char onoff[8];
	unsigned char range[8];
	unsigned char lowarm[8];
	unsigned char higharm[8];
	unsigned char timeticks[8];
	unsigned char startvalue[8];
	unsigned char pos[30];
};

struct WaterSystemConfTypedef WaterSystemConf[16];



int watersql[16][6];
int row = 0; //record row
/* callback函数中： 
 * void*,  Data provided in the 4th argument of sqlite3_exec()
 * int,    The number of columns in row 
 * char**, An array of strings representing fields in the row
 * char**  An array of strings representing column names
 */  
int callback(void *data, int nr, char **values, char **names)
{    
	int i;
	strcpy(WaterSystemConf[row].type, values[1]);
	strcpy(WaterSystemConf[row].onoff, values[2]);
	strcpy(WaterSystemConf[row].range, values[3]);
	strcpy(WaterSystemConf[row].higharm, values[4]);
	strcpy(WaterSystemConf[row].lowarm, values[5]);
	strcpy(WaterSystemConf[row].timeticks, values[6]);
	strcpy(WaterSystemConf[row].startvalue, values[7]);
	strcpy(WaterSystemConf[row].pos, values[8]);
	row++;    
	return 0;  
}
/*When open html we should read configfile to fix it*/
void ReadTandaConf(void)
{
	int rc,i;
	sqlite3 *db;  
	char *SelectSql;  
	char *zErrMsg = 0;
	const char* data = "Callback function called";
	rc = sqlite3_open("/data/.watersys.db", &db);  //打开（或新建）一个数据库   
    if( rc ){
        //fprintf(cgiOut, "Can't open databases: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        //fprintf(cgiOut, "Opened database successfully\n");
    }

	/* select sql statement*/
    SelectSql = "SELECT * from watersys";
    rc = sqlite3_exec(db, SelectSql, callback, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK ){
       // fprintf(cgiOut, "SQL select error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
       // fprintf(cgiOut, "Select successfully\n");
    }
	for(i=0 ; i<16 ; i++)
	{		
		fprintf(cgiOut, "type%d=%s/",i+1,WaterSystemConf[i].type);
		fprintf(cgiOut, "switch%d=%s/",i+1,WaterSystemConf[i].onoff);
		fprintf(cgiOut, "range%d=%s/",i+1,WaterSystemConf[i].range);
		fprintf(cgiOut, "uplimite%d=%s/",i+1,WaterSystemConf[i].higharm);
		fprintf(cgiOut, "lowlimite%d=%s/",i+1,WaterSystemConf[i].lowarm);
		fprintf(cgiOut, "rate%d=%s/",i+1,WaterSystemConf[i].timeticks);
		fprintf(cgiOut, "startvalue%d=%s/",i+1,WaterSystemConf[i].startvalue);
		fprintf(cgiOut, "location%d=%s/",i+1,WaterSystemConf[i].pos);
	}
}


int WriteConfDat(void)
{
	int channelselect[8];
	int result , invalid;
	int typechoice[8],rc;
	sqlite3 *db;  
	char sqlexec[1024];
	char *zErrMsg = 0;
	rc = sqlite3_open("/data/.watersys.db", &db);  //打开（或新建）一个数据库   
    if( rc ){
        //fprintf(cgiOut, "Can't open databases: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        //fprintf(cgiOut, "Opened database successfully\n");
    }
	
/*----------------------------------------1---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel1", ChannelSelectTable, 1, &channelselect[0], &invalid);
	cgiFormSelectSingleNormal("parametertype1", TypeSelectTable, 4, &typechoice[0], 0);
	cgiFormStringNoNewlines("range1", WaterSystemConf[0].range, sizeof(WaterSystemConf[0].range));
	cgiFormStringNoNewlines("lowarm1", WaterSystemConf[0].lowarm, sizeof(WaterSystemConf[0].lowarm));
	cgiFormStringNoNewlines("higharm1", WaterSystemConf[0].higharm, sizeof(WaterSystemConf[0].higharm));
	cgiFormStringNoNewlines("timetick1", WaterSystemConf[0].timeticks, sizeof(WaterSystemConf[0].timeticks));
	cgiFormStringNoNewlines("startvalue1", WaterSystemConf[0].startvalue, sizeof(WaterSystemConf[0].startvalue));
	cgiFormStringNoNewlines("pos1", WaterSystemConf[0].pos, sizeof(WaterSystemConf[0].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=1;",typechoice[0],channelselect[0],
		WaterSystemConf[0].range,WaterSystemConf[0].higharm,WaterSystemConf[0].lowarm,WaterSystemConf[0].timeticks,WaterSystemConf[0].startvalue,WaterSystemConf[0].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
/*----------------------------------------2---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel2", ChannelSelectTable, 1, &channelselect[1], &invalid);
	cgiFormSelectSingleNormal("parametertype2", TypeSelectTable, 4, &typechoice[1], 0);
	cgiFormStringNoNewlines("range2", WaterSystemConf[1].range, sizeof(WaterSystemConf[1].range));
	cgiFormStringNoNewlines("lowarm2", WaterSystemConf[1].lowarm, sizeof(WaterSystemConf[1].lowarm));
	cgiFormStringNoNewlines("higharm2", WaterSystemConf[1].higharm, sizeof(WaterSystemConf[1].higharm));
	cgiFormStringNoNewlines("timetick2", WaterSystemConf[1].timeticks, sizeof(WaterSystemConf[1].timeticks));
	cgiFormStringNoNewlines("startvalue2", WaterSystemConf[1].startvalue, sizeof(WaterSystemConf[1].startvalue));
	cgiFormStringNoNewlines("pos2", WaterSystemConf[1].pos, sizeof(WaterSystemConf[1].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=2;",typechoice[1],channelselect[1],\
		WaterSystemConf[1].range,WaterSystemConf[1].higharm,WaterSystemConf[1].lowarm,WaterSystemConf[1].timeticks,WaterSystemConf[1].startvalue,WaterSystemConf[1].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
/*----------------------------------------3---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel3", ChannelSelectTable, 1, &channelselect[2], &invalid);
	cgiFormSelectSingleNormal("parametertype3", TypeSelectTable, 4, &typechoice[2], 0);
	cgiFormStringNoNewlines("range3", WaterSystemConf[2].range, sizeof(WaterSystemConf[2].range));
	cgiFormStringNoNewlines("lowarm3", WaterSystemConf[2].lowarm, sizeof(WaterSystemConf[2].lowarm));
	cgiFormStringNoNewlines("higharm3", WaterSystemConf[2].higharm, sizeof(WaterSystemConf[2].higharm));
	cgiFormStringNoNewlines("timetick3", WaterSystemConf[2].timeticks, sizeof(WaterSystemConf[2].timeticks));
	cgiFormStringNoNewlines("startvalue3", WaterSystemConf[2].startvalue, sizeof(WaterSystemConf[2].startvalue));	
	cgiFormStringNoNewlines("pos3", WaterSystemConf[2].pos, sizeof(WaterSystemConf[2].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=3;",typechoice[2],channelselect[2],\
		WaterSystemConf[2].range,WaterSystemConf[2].higharm,WaterSystemConf[2].lowarm,WaterSystemConf[2].timeticks,WaterSystemConf[1].startvalue,WaterSystemConf[2].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
/*----------------------------------------4---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel4", ChannelSelectTable, 1, &channelselect[3], &invalid);
	cgiFormSelectSingleNormal("parametertype4", TypeSelectTable, 4, &typechoice[3], 0);
	cgiFormStringNoNewlines("range4", WaterSystemConf[3].range, sizeof(WaterSystemConf[3].range));
	cgiFormStringNoNewlines("lowarm4", WaterSystemConf[3].lowarm, sizeof(WaterSystemConf[3].lowarm));
	cgiFormStringNoNewlines("higharm4", WaterSystemConf[3].higharm, sizeof(WaterSystemConf[3].higharm));
	cgiFormStringNoNewlines("timetick4", WaterSystemConf[3].timeticks, sizeof(WaterSystemConf[3].timeticks));
	cgiFormStringNoNewlines("startvalue4", WaterSystemConf[3].startvalue, sizeof(WaterSystemConf[3].startvalue));		
	cgiFormStringNoNewlines("pos4", WaterSystemConf[3].pos, sizeof(WaterSystemConf[3].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=4;",typechoice[3],channelselect[3],\
		WaterSystemConf[3].range,WaterSystemConf[3].higharm,WaterSystemConf[3].lowarm,WaterSystemConf[3].timeticks,WaterSystemConf[3].startvalue,WaterSystemConf[3].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
/*----------------------------------------5---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel5", ChannelSelectTable, 1, &channelselect[4], &invalid);
	cgiFormSelectSingleNormal("parametertype5", TypeSelectTable, 4, &typechoice[4], 0);
	cgiFormStringNoNewlines("range5", WaterSystemConf[4].range, sizeof(WaterSystemConf[4].range));
	cgiFormStringNoNewlines("lowarm5", WaterSystemConf[4].lowarm, sizeof(WaterSystemConf[4].lowarm));
	cgiFormStringNoNewlines("higharm5", WaterSystemConf[4].higharm, sizeof(WaterSystemConf[4].higharm));
	cgiFormStringNoNewlines("timetick5", WaterSystemConf[4].timeticks, sizeof(WaterSystemConf[4].timeticks));
	cgiFormStringNoNewlines("startvalue5", WaterSystemConf[4].startvalue, sizeof(WaterSystemConf[4].startvalue));		
	cgiFormStringNoNewlines("pos5", WaterSystemConf[4].pos, sizeof(WaterSystemConf[4].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=5;",typechoice[4],channelselect[4],\
		WaterSystemConf[4].range,WaterSystemConf[4].higharm,WaterSystemConf[4].lowarm,WaterSystemConf[4].timeticks,WaterSystemConf[4].startvalue,WaterSystemConf[4].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
/*----------------------------------------6---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel6", ChannelSelectTable, 1, &channelselect[5], &invalid);
	cgiFormSelectSingleNormal("parametertype6", TypeSelectTable, 4, &typechoice[5], 0);
	cgiFormStringNoNewlines("range6", WaterSystemConf[5].range, sizeof(WaterSystemConf[5].range));
	cgiFormStringNoNewlines("lowarm6", WaterSystemConf[5].lowarm, sizeof(WaterSystemConf[5].lowarm));
	cgiFormStringNoNewlines("higharm6", WaterSystemConf[5].higharm, sizeof(WaterSystemConf[5].higharm));
	cgiFormStringNoNewlines("timetick6", WaterSystemConf[5].timeticks, sizeof(WaterSystemConf[5].timeticks));
	cgiFormStringNoNewlines("startvalue6", WaterSystemConf[5].startvalue, sizeof(WaterSystemConf[5].startvalue));		
	cgiFormStringNoNewlines("pos6", WaterSystemConf[5].pos, sizeof(WaterSystemConf[5].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=6;",typechoice[5],channelselect[5],\
		WaterSystemConf[5].range,WaterSystemConf[5].higharm,WaterSystemConf[5].lowarm,WaterSystemConf[5].timeticks,WaterSystemConf[5].startvalue,WaterSystemConf[5].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
/*----------------------------------------7---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel7", ChannelSelectTable, 1, &channelselect[6], &invalid);
	cgiFormSelectSingleNormal("parametertype7", TypeSelectTable, 4, &typechoice[6], 0);
	cgiFormStringNoNewlines("range7", WaterSystemConf[6].range, sizeof(WaterSystemConf[6].range));
	cgiFormStringNoNewlines("lowarm7", WaterSystemConf[6].lowarm, sizeof(WaterSystemConf[6].lowarm));
	cgiFormStringNoNewlines("higharm7", WaterSystemConf[6].higharm, sizeof(WaterSystemConf[6].higharm));
	cgiFormStringNoNewlines("timetick7", WaterSystemConf[6].timeticks, sizeof(WaterSystemConf[6].timeticks));
	cgiFormStringNoNewlines("startvalue7", WaterSystemConf[6].startvalue, sizeof(WaterSystemConf[6].startvalue));		
	cgiFormStringNoNewlines("pos7", WaterSystemConf[6].pos, sizeof(WaterSystemConf[6].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=7;",typechoice[6],channelselect[6],\
		WaterSystemConf[6].range,WaterSystemConf[6].higharm,WaterSystemConf[6].lowarm,WaterSystemConf[6].timeticks,WaterSystemConf[6].startvalue,WaterSystemConf[6].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
/*----------------------------------------8---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel8", ChannelSelectTable, 1, &channelselect[7], &invalid);
	cgiFormSelectSingleNormal("parametertype8", TypeSelectTable, 4, &typechoice[7], 0);
	cgiFormStringNoNewlines("range8", WaterSystemConf[7].range, sizeof(WaterSystemConf[7].range));
	cgiFormStringNoNewlines("lowarm8", WaterSystemConf[7].lowarm, sizeof(WaterSystemConf[7].lowarm));
	cgiFormStringNoNewlines("higharm8", WaterSystemConf[7].higharm, sizeof(WaterSystemConf[7].higharm));
	cgiFormStringNoNewlines("timetick8", WaterSystemConf[7].timeticks, sizeof(WaterSystemConf[7].timeticks));
	cgiFormStringNoNewlines("startvalue8", WaterSystemConf[7].startvalue, sizeof(WaterSystemConf[7].startvalue));		
	cgiFormStringNoNewlines("pos8", WaterSystemConf[7].pos, sizeof(WaterSystemConf[7].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=8;",typechoice[7],channelselect[7],\
	WaterSystemConf[7].range,WaterSystemConf[7].higharm,WaterSystemConf[7].lowarm,WaterSystemConf[7].timeticks,WaterSystemConf[7].startvalue,WaterSystemConf[7].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }

	/*----------------------------------------9---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel9", ChannelSelectTable, 1, &channelselect[8], &invalid);
	cgiFormSelectSingleNormal("parametertype9", TypeSelectTable, 4, &typechoice[8], 0);
	cgiFormStringNoNewlines("range9", WaterSystemConf[8].range, sizeof(WaterSystemConf[8].range));
	cgiFormStringNoNewlines("lowarm9", WaterSystemConf[8].lowarm, sizeof(WaterSystemConf[8].lowarm));
	cgiFormStringNoNewlines("higharm9", WaterSystemConf[8].higharm, sizeof(WaterSystemConf[8].higharm));
	cgiFormStringNoNewlines("timetick9", WaterSystemConf[8].timeticks, sizeof(WaterSystemConf[8].timeticks));
	cgiFormStringNoNewlines("startvalue9", WaterSystemConf[8].startvalue, sizeof(WaterSystemConf[8].startvalue));		
	cgiFormStringNoNewlines("pos9", WaterSystemConf[8].pos, sizeof(WaterSystemConf[8].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=9;",typechoice[8],channelselect[8],
		WaterSystemConf[8].range,WaterSystemConf[8].higharm,WaterSystemConf[8].lowarm,WaterSystemConf[8].timeticks,WaterSystemConf[8].startvalue,WaterSystemConf[8].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
/*----------------------------------------10---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel10", ChannelSelectTable, 1, &channelselect[9], &invalid);
	cgiFormSelectSingleNormal("parametertype10", TypeSelectTable, 4, &typechoice[9], 0);
	cgiFormStringNoNewlines("range10", WaterSystemConf[9].range, sizeof(WaterSystemConf[9].range));
	cgiFormStringNoNewlines("lowarm10", WaterSystemConf[9].lowarm, sizeof(WaterSystemConf[9].lowarm));
	cgiFormStringNoNewlines("higharm10", WaterSystemConf[9].higharm, sizeof(WaterSystemConf[9].higharm));
	cgiFormStringNoNewlines("timetick10", WaterSystemConf[9].timeticks, sizeof(WaterSystemConf[9].timeticks));
	cgiFormStringNoNewlines("startvalue10", WaterSystemConf[9].startvalue, sizeof(WaterSystemConf[9].startvalue));		
	cgiFormStringNoNewlines("pos10", WaterSystemConf[9].pos, sizeof(WaterSystemConf[9].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=10;",typechoice[9],channelselect[9],\
		WaterSystemConf[9].range,WaterSystemConf[9].higharm,WaterSystemConf[9].lowarm,WaterSystemConf[9].timeticks,WaterSystemConf[9].startvalue,WaterSystemConf[9].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
/*----------------------------------------11---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel11", ChannelSelectTable, 1, &channelselect[10], &invalid);
	cgiFormSelectSingleNormal("parametertype11", TypeSelectTable, 4, &typechoice[10], 0);
	cgiFormStringNoNewlines("range11", WaterSystemConf[10].range, sizeof(WaterSystemConf[10].range));
	cgiFormStringNoNewlines("lowarm11", WaterSystemConf[10].lowarm, sizeof(WaterSystemConf[10].lowarm));
	cgiFormStringNoNewlines("higharm11", WaterSystemConf[10].higharm, sizeof(WaterSystemConf[10].higharm));
	cgiFormStringNoNewlines("timetick11", WaterSystemConf[10].timeticks, sizeof(WaterSystemConf[10].timeticks));
	cgiFormStringNoNewlines("startvalue11", WaterSystemConf[10].startvalue, sizeof(WaterSystemConf[10].startvalue));		
	cgiFormStringNoNewlines("pos11", WaterSystemConf[10].pos, sizeof(WaterSystemConf[10].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=11;",typechoice[10],channelselect[10],\
		WaterSystemConf[10].range,WaterSystemConf[10].higharm,WaterSystemConf[10].lowarm,WaterSystemConf[10].timeticks,WaterSystemConf[10].startvalue,WaterSystemConf[10].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
/*----------------------------------------12---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel12", ChannelSelectTable, 1, &channelselect[11], &invalid);
	cgiFormSelectSingleNormal("parametertype12", TypeSelectTable, 4, &typechoice[11], 0);
	cgiFormStringNoNewlines("range12", WaterSystemConf[11].range, sizeof(WaterSystemConf[11].range));
	cgiFormStringNoNewlines("lowarm12", WaterSystemConf[11].lowarm, sizeof(WaterSystemConf[11].lowarm));
	cgiFormStringNoNewlines("higharm12", WaterSystemConf[11].higharm, sizeof(WaterSystemConf[11].higharm));
	cgiFormStringNoNewlines("timetick12", WaterSystemConf[11].timeticks, sizeof(WaterSystemConf[11].timeticks));
	cgiFormStringNoNewlines("startvalue12", WaterSystemConf[11].startvalue, sizeof(WaterSystemConf[11].startvalue));		
	cgiFormStringNoNewlines("pos12", WaterSystemConf[11].pos, sizeof(WaterSystemConf[11].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=12;",typechoice[11],channelselect[11],\
		WaterSystemConf[11].range,WaterSystemConf[11].higharm,WaterSystemConf[11].lowarm,WaterSystemConf[11].timeticks,WaterSystemConf[11].startvalue,WaterSystemConf[11].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
/*----------------------------------------13---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel13", ChannelSelectTable, 1, &channelselect[12], &invalid);
	cgiFormSelectSingleNormal("parametertype13", TypeSelectTable, 4, &typechoice[12], 0);
	cgiFormStringNoNewlines("range13", WaterSystemConf[12].range, sizeof(WaterSystemConf[12].range));
	cgiFormStringNoNewlines("lowarm13", WaterSystemConf[12].lowarm, sizeof(WaterSystemConf[12].lowarm));
	cgiFormStringNoNewlines("higharm13", WaterSystemConf[12].higharm, sizeof(WaterSystemConf[12].higharm));
	cgiFormStringNoNewlines("timetick13", WaterSystemConf[12].timeticks, sizeof(WaterSystemConf[12].timeticks));
	cgiFormStringNoNewlines("startvalue13", WaterSystemConf[12].startvalue, sizeof(WaterSystemConf[12].startvalue));		
	cgiFormStringNoNewlines("pos13", WaterSystemConf[12].pos, sizeof(WaterSystemConf[12].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=13;",typechoice[12],channelselect[12],\
		WaterSystemConf[12].range,WaterSystemConf[12].higharm,WaterSystemConf[12].lowarm,WaterSystemConf[12].timeticks,WaterSystemConf[12].startvalue,WaterSystemConf[12].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
/*----------------------------------------14---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel14", ChannelSelectTable, 1, &channelselect[13], &invalid);
	cgiFormSelectSingleNormal("parametertype14", TypeSelectTable, 4, &typechoice[13], 0);
	cgiFormStringNoNewlines("range14", WaterSystemConf[13].range, sizeof(WaterSystemConf[13].range));
	cgiFormStringNoNewlines("lowarm14", WaterSystemConf[13].lowarm, sizeof(WaterSystemConf[13].lowarm));
	cgiFormStringNoNewlines("higharm14", WaterSystemConf[13].higharm, sizeof(WaterSystemConf[13].higharm));
	cgiFormStringNoNewlines("timetick14", WaterSystemConf[13].timeticks, sizeof(WaterSystemConf[13].timeticks));
	cgiFormStringNoNewlines("startvalue14", WaterSystemConf[13].startvalue, sizeof(WaterSystemConf[13].startvalue));		
	cgiFormStringNoNewlines("pos14", WaterSystemConf[13].pos, sizeof(WaterSystemConf[13].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=14;",typechoice[13],channelselect[13],\
		WaterSystemConf[13].range,WaterSystemConf[13].higharm,WaterSystemConf[13].lowarm,WaterSystemConf[13].timeticks,WaterSystemConf[13].startvalue,WaterSystemConf[13].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
/*----------------------------------------15---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel15", ChannelSelectTable, 1, &channelselect[14], &invalid);
	cgiFormSelectSingleNormal("parametertype15", TypeSelectTable, 4, &typechoice[14], 0);
	cgiFormStringNoNewlines("range15", WaterSystemConf[14].range, sizeof(WaterSystemConf[14].range));
	cgiFormStringNoNewlines("lowarm15", WaterSystemConf[14].lowarm, sizeof(WaterSystemConf[14].lowarm));
	cgiFormStringNoNewlines("higharm15", WaterSystemConf[14].higharm, sizeof(WaterSystemConf[14].higharm));
	cgiFormStringNoNewlines("timetick15", WaterSystemConf[14].timeticks, sizeof(WaterSystemConf[14].timeticks));
	cgiFormStringNoNewlines("startvalue15", WaterSystemConf[14].startvalue, sizeof(WaterSystemConf[14].startvalue));		
	cgiFormStringNoNewlines("pos15", WaterSystemConf[14].pos, sizeof(WaterSystemConf[14].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=15;",typechoice[14],channelselect[14],\
		WaterSystemConf[14].range,WaterSystemConf[14].higharm,WaterSystemConf[14].lowarm,WaterSystemConf[14].timeticks,WaterSystemConf[14].startvalue,WaterSystemConf[14].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
/*----------------------------------------16---------------------------------------------------------*/
	result = cgiFormCheckboxMultiple("channel16", ChannelSelectTable, 1, &channelselect[15], &invalid);
	cgiFormSelectSingleNormal("parametertype16", TypeSelectTable, 4, &typechoice[15], 0);
	cgiFormStringNoNewlines("range16", WaterSystemConf[15].range, sizeof(WaterSystemConf[15].range));
	cgiFormStringNoNewlines("lowarm16", WaterSystemConf[15].lowarm, sizeof(WaterSystemConf[15].lowarm));
	cgiFormStringNoNewlines("higharm16", WaterSystemConf[15].higharm, sizeof(WaterSystemConf[15].higharm));
	cgiFormStringNoNewlines("timetick16", WaterSystemConf[15].timeticks, sizeof(WaterSystemConf[15].timeticks));
	cgiFormStringNoNewlines("startvalue16", WaterSystemConf[15].startvalue, sizeof(WaterSystemConf[15].startvalue));		
	cgiFormStringNoNewlines("pos16", WaterSystemConf[15].pos, sizeof(WaterSystemConf[15].pos));
	sprintf(sqlexec,"UPDATE watersys SET type=%d,switch=%d,range='%s',uplimite='%s',lowlimite='%s',rate='%s',startvalue='%s',location='%s' where channel=16;",typechoice[15],channelselect[15],\
	WaterSystemConf[15].range,WaterSystemConf[15].higharm,WaterSystemConf[15].lowarm,WaterSystemConf[15].timeticks,WaterSystemConf[15].startvalue,WaterSystemConf[15].pos);
	rc = sqlite3_exec(db, sqlexec, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        //fprintf(cgiOut, "SQL insert error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(cgiOut, "Records crate successfully\n");
    }
	 sqlite3_close(db); //关闭数据库   
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
			ReadTandaConf();;
		}
		
	}
	if ((cgiFormSubmitClicked("Setting") == cgiFormSuccess))
	{		
		ret=WriteConfDat();
		system("echo 6 > /data/uplog");
	}
	return 0;
}





  

