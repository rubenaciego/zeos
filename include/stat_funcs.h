#ifndef STAT_FUNCS_H
#define STAT_FUNCS_H


void stats_user_to_system();
void stats_system_to_x();
void stats_ready_to_system(struct task_struct * new);


#endif
