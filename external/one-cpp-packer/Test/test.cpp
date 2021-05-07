#include "Other.h" // include file's lines should be added
// should be removed

#include <vector> // [Ignore]
#include <set>

#include <map>

// multi line macros should remain
#define LOG(cond, msg) \
    if(cond) \
        printf(msg);

#include <string>
#include <queue>
        
/* Multi
    line
    comment
    should
    be
    removed 
*/
// Does not work with inline comment: func(arg1/*arg2*/)

//#include "all.cpp" // [Enable]
//int lolkek() { return -1; } // [Enable]

int l = 0;
#ifdef COND_DEF
int main(/*args*/)
{
    return 0;
}
#endif // COND_DEF