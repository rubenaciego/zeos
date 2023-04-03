#include <stats.h>
#include <sched.h>
#include <utils.h>

void stats_user_to_system() {
    unsigned long current_ticks = get_ticks();
    current()->stats.user_ticks += current_ticks - current()->stats.elapsed_total_ticks;
    current()->stats.elapsed_total_ticks = current_ticks;
}

void stats_system_to_x() {
    unsigned long current_ticks = get_ticks();
    current()->stats.system_ticks += current_ticks - current()->stats.elapsed_total_ticks;
    current()->stats.elapsed_total_ticks = current_ticks;
}

void stats_ready_to_system(struct task_struct * new) {
    unsigned long current_ticks = get_ticks();
    new->stats.ready_ticks += current_ticks - new->stats.elapsed_total_ticks;
    new->stats.elapsed_total_ticks = current_ticks;
    ++new->stats.total_trans;
}

void clean_stats(struct task_struct * new) {
    new->stats.user_ticks = 0;
    new->stats.system_ticks = 0;
    new->stats.blocked_ticks = 0;
    new->stats.ready_ticks = 0;
    //elapsed needs to be left as-is as it has the time the fork was called
    new->stats.total_trans = 0;
}
