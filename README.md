# C++ json parser
Alpha version.
# Example
Exist json file:

```json
{"numbers":[1,2,3,4,5,6,7,8,9,0]}
```
You can read it:
```cpp
#include<ifstream>
#include"json.hpp"
...
json j;
file >> j;
```

And you can show it:
```cpp
#include<ifstream>
#include<iostream>
#include"json.hpp"
...
json j;
file >> j;
std::cout << j;
```
