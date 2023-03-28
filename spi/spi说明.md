> 我根据ssd1306的51程序移植的，经测试无法点亮屏幕。但是数据都准确的发送了。原因未知。
> 有兴趣的可以在此基础上进行修改
+ gpio_common     关于gpio有关的操作
```cpp
int gpio_init(int gpio,DIRECTION direction,EDGE edge,char* value,int value_len);
int set_voltage(FILE * fd,int value);
int get_voltage(int fd);
void gpio_release(int gpio);
```
+ oled      关于oled屏幕的操作
> 其他的是一些测试文件