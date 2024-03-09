#include "CurrentThread.h"
#include "Thread.h"
#include <iostream>
#include <string>
#include <unistd.h>

void num(int num){
    std::cout << num << std::endl;
    //std::cout << fst::GetCurrentThreadId() << std::endl;
}

int main(){
    std::cout << "start" << std::endl;

    for(int i = 0; i < 10; i++){
        fst::Thread thread(std::bind(&num,i),"thread" + std::to_string(i));
        thread.start();
        sleep(1); 
    }

    std::cout << "end" << std::endl;

    return 0;
}