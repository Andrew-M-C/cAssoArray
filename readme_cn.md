# cAssocArray #

这是一个使用 C 语言编写的高效的关联数组类，使用红黑树原理实现。
本文是中文版的 README 文件，如果你想要英文版，请回到工程主页。


# 简介 #

cAssocArray 的目的是使用 C 语言提供一个高效的关联数组类。
我之所以要自己写一个，是因为我在开发中需要使用关联数组，但是找遍网络也找不到一个轻量级的、可用的、高效的实现——不像 cJSON 那样，名气很大而且用得很多。所以，我干脆自己写一个好了。

# 用法 #

想要使用，只需要将 cAssocArray.c/h 两个文件拷贝到自己的工程里面就好，然后执行 make 操作。请注意在连接的时候需要加入 "`-lpthread`" 选项。

以下是本类的几个函数接口。所有函数在错误发生的时候都会设置 `errno` 的值。

---

### `cAssocArray_Create()` ###

这个函数创建一个空的 `cAssocArray` 类。参数 **locked** 的意思是是否自动在类的访问接口中对类加锁。如果你不是在多线程环境中使用，或者是自己可以确保不会出现重入问题，则指定 `FALSE`。

执行成功时返回一个关联数组对象，否则返回 NULL。

---

### `cAssocArray_Delete()` ###

调用这个函数，释放掉关联数组中的所有对象，并且释放掉数组本身。

---

### `cAssocArray_SetValue()` ###

设置指定的键值对。成功时返回 0，否则返回 -1。以下是各参数的说明：

 - `array`: 关联数组对象
 - `key`: 键值对的 “键”。不能为 NULL 或空字符串。
 - `value`: 键值对的 “值”，地址值。不能为 NULL。
 - `freeDuplicate`: 当指定的键值对已经存在于关联数组中，并且旧的 “值” 与新值不想等，指定该参数为 `TRUE`，本关联数组会先 `free` 掉旧值，再赋新值。否则，只是简单地赋新值。

---

### `cAssocArray_GetValue()` ###
 
获取指定键值对中的 “值”。
 
 ---
 
### `cAssocArray_Size()` ###

获取关联数组中键值对的数量。 

---
### `cAssocArray_RemoveValue()` ###

删除指定的键值对。当参数 `shouldFree` 为 TRUE 时，关联数组会对目标值调用 `free()`。
如果指定的键不存在，则返回 -1，并且 `errno` 会被设置为 `ENOENT`。



---
### `cAssocArray_DetachValue()` ###

将指定的键值对从关联数组中删除掉，但不会调用 free。被删除的键值对的值会被返回给调用者。如果键值对不存在，则返回 NULL。

---
### `cAssocArray_AddValue()` ##

和函数 `cAssocArray_SetValue()` 相同，不同的是如果键已经存在了，则什么都不做，并且返回错误。

---
### `cAssocArray_UpdateValue()` ###

和函数 `cAssocArray_SetValue()` 相同，不同的是如果键如果不存在，则什么都不做，并且返回错误。

---
### `cAssocArray_DumpToStdout()` ###

调试用，将关联数组的内容输出到 stdout 中。正式应用中，请不要调用这个函数。

# 下一步功能 #

1. 键值对中支持空值。考虑使用一个 `cEmptyValue` 来表示，与表示错误的 `NULL` 相区分。

# 联系作者 #

如果有建议或者 bug，请[发邮件给我](mailto://laplacezhang@126.com)。如果能够附上 bug 的重现步骤的话就更好了。





