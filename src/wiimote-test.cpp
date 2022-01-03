#include <stdio.h>
#include <string.h>
//#include <xwiimote.h>

#include <thread>
#include <unistd.h>


class Input{
public:
	Input(){}
	
	static void threadedFunc(Input *i){
		while (i->running){
			i->buttons.X++;
			i->buttons.Y++;
			i->buttons.Z++;
			sleep(1);
		}
		printf("ended");
	}
	void reset(){
		buttons={0,0,0};
	}
	void start(){
		running = true;
		std::thread inputThread(&Input::threadedFunc, this);
		//inputThread.detach();
	}
	void stop(){
		running = false;
	}
	void read(){
		printf("%ld, %ld,%ld", buttons.X,buttons.Y,buttons.Z);
	}
	
	struct{
		unsigned long int X,Y,Z;
	} buttons;
	bool running;
};


int main(){
	Input test;
	test.reset();
	//std::thread inputThread(&Input::threadedFunc, &test);
	test.start();
	bool running=true;
	
	while(running){
	
		char strInput[50];
		scanf("%s", strInput);
		
		if (strcmp(strInput, "stop")==0){
			running=false;
			test.stop();
		}
		if (strcmp(strInput, "read")==0){
			test.read();
			test.reset();
		}
		printf("\n");
		
	}
	//inputThread.join();

}
