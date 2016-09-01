#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char *argv[] )
{
    
	char *UserInput = NULL; 		// index the user input data.
   	char *request_method = NULL; 	//index the html transfer type.
	int data_len = 0;		// index the input data's length.

    printf("Content-type:text/html;charset=utf-8\n\n"); //response header.
    printf("\n");

    // HTML page header.
    printf("<html><head>");
    printf("<title>%s</title></head>", "");
    printf("<body>");

	if( ! getenv("REQUEST_METHOD") )
	{
		printf("No client request from user !\n");
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
        //printf("method = %s, data_len = %d<br>", request_method, data_len);
        //printf("data: %s", UserInput);
		if(!strcmp(&UserInput[7],"1357"))
		{			
			//printf("<meta http-equiv=\"Refresh\" content=\"0;URL=./config.cgi\"> ");
			printf("<script>window.location.href='/config.html';</script>");
			printf( "There's no input data !\n" );
		}
		else
		{
			printf("key err<BR>\nyou input key is:<BR>\n %s",&UserInput[7]);
		}
	}		
    printf("</body></html>");
    fflush( stdout );
    return 0;
}


