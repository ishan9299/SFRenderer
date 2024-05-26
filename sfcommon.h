/* date = February 13th 2024 2:15 pm */

#ifndef SFCOMMON_H
#define SFCOMMON_H

#define TYPE_PUN(var, var_type_to) *(var_type_to *)&var
#define SWAP(T, x, y) do {T t = x; x = y; y = t;} while(0)

#endif //SFCOMMON_H
