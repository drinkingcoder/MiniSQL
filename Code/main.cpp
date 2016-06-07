//
//  main.cpp
//  MiniSQL-final
//
//  Created by drinking on 10/26/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#include <iostream>
#include "API.hpp"

int main(int argc, const char * argv[]) {
    API* api = new API();
    api->start();
}
