# cAssocArray #

Efficient Associative Array in C, implemented with Red-Black Tree.
This is the main README file in English. For simplified Chinese version, please press [here](./readme_cn.md). 

中文版 README 请[点击这里](./readme_cn.md)。

# Description #

cAssocArray aims to implement an efficient associated array in C. As I found no applicatable C associative array, I decided to write one by myself.

# Usage #

Just copy two files to the project repo you needed: cAssocArray.c/h, and then make. Please add "`-lpthread`" flag when linking your target files.

There are several interface functions you may need to know. All function will set `errno` if error occurs.

---

### `cAssocArray_Create()` ###

This function take a parameter and return a applicatable empty associated array.
The parameter **locked** indicated that whether the array should automatically lock itself when accessing it. If you do not have muti-thread access to this array, please specify "FALSE".

Return an array if success, NULL if error occurs.

---

### `cAssocArray_Delete()` ###

Invoking this function to release all members in the associative array and remove 
the existence itself from the memory.

---

### `cAssocArray_SetValue()` ###

Set a value for a specified key. If successful, returns 0, otherwise -1. 
Parameter descriptions:

 - `array`: Given array
 - `key`: Key for the value. Should NOT be NULL or empty string.
 - `value`: Value(an address value) for the key. Should not be NULL.
 - `freeDuplicate`: When specified key has existed before this invoke, and the value was not equal to the new one, cAssocArray will `free` it if this parameter is `TRUE`. Otherwise, just replace the value with new one.

---

### `cAssocArray_GetValue()` ###
 
Get the value (an address value) for specified key. NULL will be returned if the value does not exist.
 
 ---
 
### `cAssocArray_Size()` ###
 
Returns how many key-value pairs the array has.

---
### `cAssocArray_RemoveValue()` ###

Remove specified key-value pair. The parameter "`shouldFree`" decides whether the array frees the value.

If specified key-value pair does not exist, -1 will be returned and `errno` set as `ENOENT`.

---
### `cAssocArray_DetachValue()` ###

Remove specified key-value pair from array WITHOUT freeing the value. Instead, the value (an address value) will be returned to invoker. 

NULL will be returned if the key-value does not exist.

---
### `cAssocArray_AddValue()` ##

Same as `cAssocArray_SetValue()`, but this function will fail and return error if specified key already exists in the array.

---
### `cAssocArray_UpdateValue()` ###

Same as `cAssocArray_SetValue()`, but this function will fail and return error if specified key-value does not exist.

---
### `cAssocArray_DumpToStdout()` ###

Debug use for **cAssocArray**. This will print members in the array to console. Please do not invoke this function in formal project.

# TODO #

1. Support "empty value" in associative array. I am considering using a value called "cEmptyValue" instead of "NULL".
2. Support `cAssocArray_GetKeys()`


# Contact Me #

If finding any bugs or suggestions, please [send me a Email](mailto://laplacezhang@126.com). I will be very thankful if you attach bug replicate steps.





