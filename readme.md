

<!-- 大小根堆 -->

# 库的基本功能说明：
> 封装了嵌入式中常用的数据结构和算法
> 包括四个部分：
> 1. 常用数据结构的库里面包括了链表,普通队列,优先队列,栈等数据类型
> 2. 图结构算法(你可能看到有些是空的,我想我现在没什么需求,如果有需求,后面就加上吧)
> 3. 常用排序算法
> 4. 字符匹配算法KMP

## 功能：

#### 1. 树形菜单
> 更新:增加了树形菜单核心库和参考使用,测试案例base.c,核心库是menuBase.c和menuBase.h,userPage为用户自定义页面
> V1.0.1 split测试分支说明,在最新的实验分支split上基本上推翻了现在master上的方法，结合了状态机页面分离和链表，这样功能更容易做到，并且丰富，建议使用这种方式，做到这里觉得有些无聊，所以要搁置了

**说明：**
V1.0.0 单使用链表时功能：
    1. 支持参数绑定设置,可用用来调pid等参数
    2. 支持内置游戏绑定,切换游戏页面和退出
    3. 支持开关，单选和多选
    4. 新增了按键缓冲，给编辑模式下的回调函数使用，这一方法大大简化了逻辑，使得上面的三个功能得到简化
按键要求：
    1. 上
    2. 下
    3. 进入
    4. 退出
    5. 确认

**注意：**
    使用:使用gcc编译器,用msvc会提示错误
#### 2. 软件定时器
    ./thread_timer目录

#### 3. adafruit直线,圆,矩形,圆角矩形的移植
    ./Adafruit_transplant

#### 4. A*算法c实现
    ./Graph_Algorithm_lib

#### 5. 圆圈碰撞物理反弹模拟
    ./Ball_Coll
## 驱动：
#### 1. st7735 lcd驱动
    ./ST7735_driver目录

#### 2. 单IO口adc实现多键,单击双击多击,长按实现逻辑
    ./input_manager
#### 3. RGB_LED 21/7/24
    增加spi驱动rgb灯驱动

#### 4. 增加EC11编码器驱动 21/10/8


### 2024/5/17
#### 5. pid控制
#### 6. 电机运动控制
#### 3. 串口/ble/usb缓冲读写

## 测试：
#### 测试文件
> 测试文件test.c,仅是用作对各个模块的一个使用测试,实际库中并不需要添加



## 使用说明：
> 使用的时候只需要把对应的头文件或源文件放进目录里