

#include "Timestamp.h"
#include <iostream>
int main(){
    std::cout << fst::Timestamp::now().toString() << std::endl;
    return 0;
}