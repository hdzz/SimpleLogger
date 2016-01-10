# SimpleLogger
Simple Logger，version 0.1.1

只支持字符串类型变量，输出格式类似pringf 

参考StackOverflow上的答案实现打印调用者和行数 https://stackoverflow.com/questions/353180/how-do-i-find-the-name-of-the-calling-function/378165#378165

利用C++11新关键字thread_local，实现各线程存储各自的调用函数和行数变量
利用C++11新类型atomic_flag实现简单的自旋锁，确保对同一流输出时日志打印同步
