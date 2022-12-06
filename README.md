# JsonToCppStruct

This is a small library to copy json data into c++ structs

## Install

Simply copy the header and the source file in your project

## Usage

* Include the header file

```cpp
#include "JsonReader.h"
```

* You can use macros to save time. Enable them with

```cpp
#define JSON_USE_MACROS
```


* Declare your structure(s)

```cpp
struct jsonstruct {
    int a;
    char* string;
};
 
jsonstruct* s = new jsonstruct[4]
```

### With macros

```cpp
JSON__OPEN("filename");
  
JSON__TYPE(jsonstruct)
JSON__FIELD(a, int);
JSON__FIELD(string, char*);
JSON__ENDTYPE
  
JSON__ROOTFIELD(data, jsonstruct, 0, 4);
  
JSON__OUT(s);
  
JSON__CLOSE
```

### Without macros

```cpp
JsonReader jr("filename";
 
jr.addObjectType("jsonstruct");
jr.setObjectField("a", "int", sizeof(int), offsetof(jsonstruct, a));
jr.setObjectField("string", "char*", sizeof(char*), offsetof(jsonstruct, string));
  
jr.setObjectField("root", "data", "jsonstruct", sizeof(jsonstruct), 0, 4);
  
jr.writeObject((void*)s);
```

## Contact

for further questions, contact me at clement.chambard@universite-paris-saclay.fr
