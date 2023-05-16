cocos0 : fase1/cocos0.c librerias/winsuport.o librerias/winsuport.h
	gcc -Wall fase1/cocos0.c librerias/winsuport.o -o fase1/cocos0 -lcurses

cocos1 : fase1/cocos1.c librerias/winsuport.o librerias/winsuport.h
	gcc -Wall fase1/cocos1.c librerias/winsuport.o -o fase1/cocos1 -lcurses -lpthread

cocos2 : fase2/cocos2.c librerias/winsuport.o librerias/winsuport.h
	gcc -Wall fase2/cocos2.c librerias/winsuport.o -o fase2/cocos2 -lcurses -lpthread

cocos3 : fase3/cocos3.c fase3/fantasma3.c librerias/winsuport2.o librerias/winsuport2.h librerias/memoria.o librerias/memoria.h
	gcc -Wall fase3/cocos3.c librerias/winsuport2.o -o fase3/cocos3 -lcurses -lpthread
	gcc -Wall fase3/fantasma3.c librerias/winsuport2.o -o fase3/fantasma3 -lcurses

winsuport.o : librerias/winsuport.c librerias/winsuport.h
	gcc -Wall -c librerias/winsuport.c -o librerias/winsuport.o 

winsuport2.o : librerias/winsuport2.c librerias/winsuport2.h
	gcc -Wall -c librerias/winsuport2.c -o librerias/winsuport2.o

memoria.o : librerias/memoria.c librerias/memoria.h
	gcc -Wall -c librerias/memoria.c -o librerias/memoria.o

clean:
	rm -f fase1/cocos0 fase1/cocos1 fase2/cocos2 fase3/cocos3 librerias/winsuport.o librerias/memoria.o librerias/winsuport2.o
