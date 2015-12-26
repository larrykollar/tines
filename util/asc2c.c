#include <stdio.h>

#define bufsize 4096

void asc2c (FILE *file_in,FILE *file_out,int cmpr){
	char buf[100];
	char prev;
	int bpos;
	while(fgets(buf,100,file_in)!=NULL){
		bpos=0;
		while(buf[bpos]){
			switch(buf[bpos]){
				case 13:case ' ':case 10:case '\t':
					if(cmpr){
						if(prev!=' '&&prev!='>')fprintf(file_out," ");
						prev=' ';
					} else {
						switch(buf[bpos]){
							case ' ':fprintf(file_out," ");break;
							case '\t':fprintf(file_out,"	");break;
							case 10:fprintf(file_out,"\\n\\\n");break;
						}
					}
				break;
				case '\\':fprintf(file_out,"\\\\");prev=buf[bpos];break;
				case '\"':fprintf(file_out,"\\\"");prev=buf[bpos];break;
				default:  fprintf(file_out,"%c",buf[bpos]);
					prev=buf[bpos];
				break;
			}
		bpos++;
		}
	}
}
int main(int argc,char **argv){
	asc2c(stdin,stdout,argc-1);
	return 0;	
}
