

无符号数溢出问题
https://blog.csdn.net/wenbodong/article/details/110223168



数据结构
数组：固定
链表：动态
堆：接触少。如果要用堆，可以参考libev。

               启动定时器         停止定时器(删除链表节点)           轮询定时器
数组         O(n) 从数组找空闲点     O(1)               
单向无序链表  O(1)添加到链表头        O(n)轮询         O(n)轮询整个链表(loop中删除定时器不需要轮询)
双向无序链表  O(1)                   O(1)             O(n)轮询整个链表(loop中删除定时器不需要轮询)
有序双向链表  O(n)                   O(1)             不需要轮询全部链表(周期定时器重新插入，需要O(n))


如果程序只需要几个定时器，甚至还有一些周期定时器只是初始化后就不会再改动。
那么，用无序链表，起始更合适。毕竟，轮询十几个节点也不需要多少时间。

如果在创建定时器的时候，就把其加入链表，停止定时器，不操作链表。
之后删除的时候才从链表剔除，而且是从LOOP中剔除，是不是就可以不加锁了？