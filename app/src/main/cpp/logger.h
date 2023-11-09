//#ifndef LOGGER_H
//#define LOGGER_H

#include <string>

class logger{
public:
	logger(){
		logged = false;
	}
	int outputLog(std::string log, std::string filepath) {
		if(!logged){
			logged = true;
			FILE* pFile = fopen(filepath.c_str(), "w+");//  /sdcard instead of /storage/emulated/0
			//FILE* pFile = fopen("/storage/emulated/0/Android/data/com.SPH2/files/log.txt", "w+");//  /sdcard instead of /storage/emulated/0
			if (pFile != NULL) {
				fputs((log + "eof").c_str(), pFile);
				fclose(pFile);
			}
		}
		return 0;
	}
	bool logged;
private:
} myLogger;

//#endif