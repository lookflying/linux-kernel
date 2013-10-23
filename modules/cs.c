#include <../kernel/sched/sched.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
MODULE_LICENSE("GPL");
#ifdef CONFIG_SMP
static int
select_task_rq_cs(struct task_struct *p, int sd_flag, int flags)
{
	return task_cpu(p); /* IDLE tasks as never migrated */
}

static void pre_schedule_cs(struct rq *rq, struct task_struct *prev)
{
/*	idle_exit_fair(rq);
	rq_last_tick_reset(rq);*/
}

static void post_schedule_cs(struct rq *rq)
{
	idle_enter_fair(rq);
}
#endif /* CONFIG_SMP */
/*
 * Idle tasks are unconditionally rescheduled:
 */
static void check_preempt_curr_cs(struct rq *rq, struct task_struct *p, int flags)
{
	resched_task(rq->idle);
}

static struct task_struct *pick_next_task_cs(struct rq *rq)
{
	schedstat_inc(rq, sched_goidle);
#ifdef CONFIG_SMP
	/* Trigger the post schedule to do an idle_enter for CFS */
	rq->post_schedule = 1;
#endif
	return rq->idle;
}

/*
 * It is not legal to sleep in the idle task - print a warning
 * message if some code attempts to do it:
 */
static void
dequeue_task_cs(struct rq *rq, struct task_struct *p, int flags)
{
	raw_spin_unlock_irq(&rq->lock);
	printk(KERN_ERR "bad: scheduling from the idle thread!\n");
	dump_stack();
	raw_spin_lock_irq(&rq->lock);
}

static void put_prev_task_cs(struct rq *rq, struct task_struct *prev)
{
}

static void task_tick_cs(struct rq *rq, struct task_struct *curr, int queued)
{
}

static void set_curr_task_cs(struct rq *rq)
{
}

static void switched_to_cs(struct rq *rq, struct task_struct *p)
{
	BUG();
}

static void
prio_changed_cs(struct rq *rq, struct task_struct *p, int oldprio)
{

}

static unsigned int get_rr_interval_cs(struct rq *rq, struct task_struct *task)
{
	return 0;
}

/*
 * Simple, special scheduling class for the per-CPU idle tasks:
 */
struct sched_class cs_sched_class = {
	/* .next is NULL */
	/* no enqueue/yield_task for idle tasks */

	/* dequeue is not valid, we print a debug message there: */
	.dequeue_task		= dequeue_task_cs,

	.check_preempt_curr	= check_preempt_curr_cs,

	.pick_next_task		= pick_next_task_cs,
	.put_prev_task		= put_prev_task_cs,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_cs,
	.pre_schedule		= pre_schedule_cs,
	.post_schedule		= post_schedule_cs,
#endif

	.set_curr_task          = set_curr_task_cs,
	.task_tick		= task_tick_cs,

	.get_rr_interval	= get_rr_interval_cs,

	.prio_changed		= prio_changed_cs,
	.switched_to		= switched_to_cs,
};
static int __init cs_sched_init(void){
	printk(KERN_INFO "cs sched inited\n");
	cs_sched_class.next = &fair_sched_class;
	return 0;
}

static void __exit cs_sched_exit(void){
	printk(KERN_INFO "cs sched exited\n");
}

module_init(cs_sched_init);
module_exit(cs_sched_exit);

