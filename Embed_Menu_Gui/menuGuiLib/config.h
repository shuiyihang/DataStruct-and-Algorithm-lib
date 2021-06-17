#ifndef __CONFIG_H__
#define __CONFIG_H__


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    root,
    about,
    bluetooth,//单开关测试
    setParam,//参数设置测试
    multiSelt,//多选测试
    game,//游戏测试
    // selt1,
    // selt2,
    // selt3,
}NodeID;


#define NODE_MAX 15


#ifdef __cplusplus
}
#endif

#endif