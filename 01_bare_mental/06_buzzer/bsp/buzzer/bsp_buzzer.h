#ifndef BSP_BUZZER_H__
#define BSP_BUZZER_H__

#ifdef __cplusplus
extern "C" {
#endif

void buzzer_init(void);
void buzzer_switch(int status);

#ifdef __cplusplus
}
#endif

#endif // BSP_BUZZER_H__
