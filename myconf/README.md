服务端框架

基于epoll。

1个master线程负责accept，N个worker线程负责处理连接。

默认master采用轮转的方式分配新连接，分配策略可以自定义。

适合长连接场景，如果是高并发短连接，一个master可能处理不过来；

那么就需要修改为多个master线程负责accept。

Worker线程内可以自定义添加IO事件。同时支持定时器。

纯C语言实现，不依赖外部库，仅支持GCC + X86_64 + Linux。
