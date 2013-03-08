/* *
 * @file:   rtx.h
 * @brief:  User API prototype, this is an example only
 * @author: Yiqing Huang
 * @date:   2013/01/12
 */
#ifndef _RTX_H
#define _RTX_H

typedef unsigned int U32;

#define __SVC_0  __svc_indirect(0)

extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
int __SVC_0 _release_processor(U32 p_func);

extern int k_block_and_release_processor(void);
#define block_and_release_processor() _block_and_release_processor((U32)k_block_and_release_processor)
int __SVC_0 _block_and_release_processor(U32 p_func);

extern int k_set_priority(int priority, int target);
#define set_priority(priority, target) _set_priority((U32)k_set_priority, priority, target)
int __SVC_0 _set_priority(U32 p_func, int priority, int target);

extern int k_set_my_priority(int priority);
#define set_my_priority(priority) _set_my_priority((U32)k_set_my_priority, priority)
int __SVC_0 _set_my_priority(U32 p_func, int priority);

extern int k_cmd_register(const char *tag, void (*func)(const char *args));
#define cmd_register(tag, priority) _cmd_register((U32)k_cmd_register, priority)
int __SVC_0 _cmd_register(U32 p_func, const char *tag, void (*func)(const char *args));

#endif /* !_RTX_H_ */
