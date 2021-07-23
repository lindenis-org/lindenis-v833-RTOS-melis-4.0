**MIPS和ARM  Cache操作对应关系:**

驱动开发过程中，一般会遇到cache的两种操作:
1.无效cache line, cache  line中的数据无论新旧，都会被标记为垃圾数据，有效数据依然保留在内存中,cache line中的数据是无效数据。
2.刷cache line. 如果cache中的数据是修改后的新数据，会把数据同步写入到主存中，对应cache line中的数据仍然是有效数据，否则
   如果数据不脏，则数据已同步，默认仍是最新。
   	
         
***MIPS架构:***
    接口中的 flush操作对应上面第二项。
    invalidate操作，对应上面第一项
   
***ARM架构:***
    arm架构用clean代替了flush, flush代替了invalidate.
    所以对应关系是这个样子的
    
      MIPS                        ARM
      
      flush<-------------->clean
      inval<-------------->flush