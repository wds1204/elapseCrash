# elapseCrash

android's java_crash &amp; native_crash monitor

TODO...

```
User
typedef struct {
  /* Pathname of shared object that contains address. */
  const char* dli_fname;
  /* Address at which shared object is loaded. */
  void* dli_fbase;
  /* Name of nearest symbol with address lower than addr. */
  const char* dli_sname;
  /* Exact address of symbol named in dli_sname. */
  void* dli_saddr;
} Dl_info;
```

`Dl_info`这个结构体定义，通常在编程中用于获取动态链接库(shared object，或称为共享库)中的符号信息。包含的一下成员：

1. `dli_fname`:
   1. 类型：`const char*`
   2. 描述：指向包含地址的共享对象的路径名(文件名）的指针
2. `dli_fbase`:
   1. 类型：`const char*`
   2. 描述：指向共享对象加载到内存中的基地址的指针。即共享对象在内存中的起始地址
3. `li_sname`：
   1. 类型：`const char*`
   2. 描述:
4. `dli_saddr`：
   1. 类型：void*
   2. 描述：指向符号 `dli_sname` 的确切地址的指针。


