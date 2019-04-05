/*
NNS @ 2018
pcsx-rearmed-launcher
This launcher allow to use retroarch memcard into PCSX-ReARMed standalone.
*/

const char programversion[]="0.1a"; //program version

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <limits.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>



char *gamepath; //path to game image
char memcard_blank[]="/opt/retropie/emulators/pcsx-rearmed/blankcard.mcd"; //path to blank memcard
char memcard_blank_rescue[]="/opt/retropie/configs/psx/pcsx/memcards/card9.mcd"; //path to "rescue" blank memcard
char memcard_pcsx[]="/opt/retropie/configs/psx/pcsx/memcards/card1.mcd"; //path to pcsx memcard
char memcard_game[PATH_MAX]; //path to game memcard
char commandline_temp[PATH_MAX]; //use as a buffer to build command line
char commandline_buffer[PATH_MAX]; //use as a buffer to build command line
struct stat filestat; //use to check if memcard in pcsx folder is a symlink
bool memcard_pcsx_real=false; //use to restore pcsx memcard after
int strlength=0; //use to found if variable contain space
bool strspace=false; //use to found if variable contain space


void show_usage(void){
	fprintf(stderr,
"Version: %s\n"
"Example: ./pcsx-rearmed-launcher -cdfile \"/home/pi/RetroPie/roms/psx/GAME.img\"\n"
"Options:\n"
"\t-cdfile, Full path to game image\n"
"Note: Additional arguments will be pass through to PCSX run command line\n"
,programversion);
}

int main(int argc, char *argv[]){ //main
	for(int i=1;i<argc;++i){ //argument to variable
		if(strcmp(argv[i],"-help")==0){show_usage();return 1;
		}else if(strcmp(argv[i],"-cdfile")==0){gamepath=(char*)argv[i+1]; if(access(gamepath,R_OK)!=0){fprintf(stderr,"pcsx-rearmed-launcher : Failed, \"%s\" not readable\n",gamepath);return 1;}}
	}
	
	//check if cdfile provided
	if(gamepath==NULL){fprintf(stderr,"pcsx-rearmed-launcher : Failed, game path not specified, use -cdfile argument to do so\n");show_usage();return 1;
	}else{fprintf(stderr,"pcsx-rearmed-launcher : Game path : \"%s\"\n",gamepath);}
	
	//check if "blank" memcard exist, used if current game don't have it own
	if(access(memcard_blank,R_OK)!=0){
		fprintf(stderr,"pcsx-rearmed-launcher : Blank memcard not readable : \"%s\", trying to create a new one using \"%s\"\n",memcard_blank,memcard_blank_rescue);
		sprintf(commandline_temp,"cp \"%s\" \"%s\"",memcard_blank_rescue,memcard_blank); //build command line to copy game memcard
		system(commandline_temp); //run command
		if(access(memcard_blank,R_OK)!=0){fprintf(stderr,"pcsx-rearmed-launcher : Failed\n");return 1;
		}else{fprintf(stderr,"pcsx-rearmed-launcher : Blank memcard created with success : \"%s\"\n",memcard_blank);}
	}else{
		fprintf(stderr,"pcsx-rearmed-launcher : Blank memcard found : \"%s\"\n",memcard_blank);
	}
	
	//check if game memcard exist, copy blank memcard if not
	strcpy(memcard_game,gamepath); //copy game image path to game memcard path
  *strrchr(memcard_game, '.')='\0'; //remove game image extension
	strcat(memcard_game,".srm"); //add memcard extension
	if(access(memcard_game,R_OK)!=0){
		fprintf(stderr,"pcsx-rearmed-launcher : Game memcard not readable (\"%s\"), trying to create a new one\n",memcard_game);
		sprintf(commandline_temp, "cp \"%s\" \"%s\"",memcard_blank,memcard_game); //build command line to copy game memcard
		system(commandline_temp); //run command
		if(access(memcard_game,R_OK)!=0){fprintf(stderr,"pcsx-rearmed-launcher : Failed\n");return 1;
		}else{fprintf(stderr,"pcsx-rearmed-launcher : Game memcard created with success : \"%s\"\n",memcard_game);}
	}else{
		fprintf(stderr,"pcsx-rearmed-launcher : Game memcard found : \"%s\"\n",memcard_game);
	}
	
	//check if card1.mcd in pcsx folder is a symlink or a real file
	if(access(memcard_pcsx,R_OK)!=0){fprintf(stderr,"pcsx-rearmed-launcher : PCSX memcard not readable : \"%s\"\n",memcard_game);}
	lstat(memcard_pcsx,&filestat); //lstat pcsx memcard
	if(S_ISREG(filestat.st_mode)){ //is not a symlink
		sprintf(commandline_temp, "mv \"%s\" \"%s.bak\"",memcard_pcsx,memcard_pcsx); //build command line to backup pcsx memcard, overwrite if backup pcsx memcard already exist
		system(commandline_temp); //run command
		memcard_pcsx_real=true;
		fprintf(stderr,"pcsx-rearmed-launcher : PCSX memcard backup to \"%s.bak\"\n",memcard_pcsx);
	}else if(S_ISLNK(filestat.st_mode)){ //is a symlink
		sprintf(commandline_temp, "rm \"%s\"",memcard_pcsx); //build command line to pcsx memcard symlink
		system(commandline_temp); //run command
		fprintf(stderr,"pcsx-rearmed-launcher : Previous symlink removed\n");
	}
	
	//create symlink
	sprintf(commandline_temp, "ln -s \"%s\" \"%s\"",memcard_game,memcard_pcsx); //build command line to create symlink
	system(commandline_temp); //run command
	lstat(memcard_pcsx,&filestat); //lstat pcsx memcard
	if(S_ISLNK(filestat.st_mode)&&access(memcard_pcsx,R_OK)==0){fprintf(stderr,"pcsx-rearmed-launcher : Symlink created with success\n"); //is a symlink
	}else{ //fail to create 
		fprintf(stderr,"pcsx-rearmed-launcher : Failed to create symlink, PCSX memcard will be used : \"%s\"\n",memcard_pcsx);
		if(memcard_pcsx_real){
			sprintf(commandline_temp, "mv \"%s.bak\" \"%s\"",memcard_pcsx,memcard_pcsx); //build command line to restore backup pcsx memcard
			system(commandline_temp); //run command
			memcard_pcsx_real=false;
			fprintf(stderr,"pcsx-rearmed-launcher : PCSX original memcard restored\n");
		}
	}
	
	//build pcsx run command
	strcpy(commandline_temp,"./pcsx"); //copy game image path to game memcard path
	for(int i=1,j=0;i<argc;++i){ //add argument to pcsx run command
		strspace=false; //reset space found boolean
		strlength=strlen(argv[i]);
		while(j<strlength){ //check if argument contain space
			if(argv[i][j++]==' '){
				strspace=true; //space found boolean
				sprintf(commandline_buffer, " \"%s\"",argv[i]); //build temporary char array
				strcat(commandline_temp,commandline_buffer); //add variable to command with quote
				break;
			}
		}
		
		if(!strspace){
			sprintf(commandline_buffer, " %s",argv[i]); //build temporary char array
			strcat(commandline_temp,commandline_buffer); //add argument to command
		}
	}
	
	//start pcsx
	fprintf(stderr,"pcsx-rearmed-launcher : Running PCSX : %s\n",commandline_temp);
	system(commandline_temp); //run command
	
	//pcsx closed
	fprintf(stderr,"pcsx-rearmed-launcher : PCSX closed\n");
	
	//restore pcsx memcard
	if(memcard_pcsx_real){
		sprintf(commandline_temp, "mv \"%s.bak\" \"%s\"",memcard_pcsx,memcard_pcsx); //build command line to restore backup pcsx memcard
		system(commandline_temp); //run command
		fprintf(stderr,"pcsx-rearmed-launcher : PCSX original memcard restored\n");
	}
	
	return(0);
}