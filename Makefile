cc=arm-fsl-linux-gnueabi-gcc

all:config.cgi login.cgi

config.cgi:config.c cgic.c cgic.h
	$(cc) -o config.cgi cgic.c config.c

login.cgi:login/login.c
	$(cc) -o login.cgi  login/login.c

