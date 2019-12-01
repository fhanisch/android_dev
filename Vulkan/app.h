#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <string.h>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#ifdef __cplusplus
class App {
    
public:
    App();
    ~App();
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

void startApp(FILE* file);

#ifdef __cplusplus
}
#endif


#endif  // APP_H