# SimpleLogger  
Simple Logger, version 0.1.2, require c++11  
printf formatting rule~  

1、利用C++11新关键字thread_local，实现各线程存储各自的调用函数和行数变量  
2、利用C++11新类型atomic_flag实现简单的自旋锁，确保对同一流输出时日志打印同步  
3、有带缓冲和不带缓冲两种方式写入日志文件  
4、创建log时可以选择最小日志级别和指定输出文件，支持多个线程对同一输出文件进行输出  

参考stackoverflow上答案输出函数名和行数：https://stackoverflow.com/questions/353180/how-do-i-find-the-name-of-the-calling-function/378165#378165  
