### CYSE 570 Final Project
To view a particular scenario, go into `src/config.h` and change the following code block:

```c
// Which test scenario kernel_main loads (programs/<name>/<name>.c)
#define LOGOS_SCENARIO_CONTROL 0      // 3 tasks: never deadlocks
#define LOGOS_SCENARIO_ABBA 1         // 2 tasks: always deadlocks in the first cycle
#define LOGOS_SCENARIO_RACY 2         // 2 tasks: eventually deadlocks
#define LOGOS_SCENARIO_PHILO 3        // 5 dining philosophers: 5-task cycle to deadlock
#define LOGOS_SCENARIO_EXITLOCK 4     // 3 tasks: exit while holding locks to release them
#define LOGOS_SCENARIO_POOL 5         // 3 tasks: deadlocks with no second resource (3-unit pool)
#define LOGOS_SCENARIO_SAFECYCLE 6    // 3 tasks: wait-for cycle that isn't deadlock due to multiple resources

#define LOGOS_SCENARIO [SCENARIO_HERE]
```

For example, to run the control scenario, you would change the `LOGOS_SCENARIO` to:

```c
#define LOGOS_SCENARIO LOGOS_SCENARIO_CONTROL
```

And to do Dining Philosophers, you would:

```c
#define LOGOS_SCENARIO LOGOS_SCENARIO_PHILO
```

To view the results, `make clean; make`, and then `make run-os`. It is highly important to `make clean` prior to `make`, as `make` does not always account for changes in header files. If you would like to take advantage of QEMU commands such as `pause` and `cont` to pause and continue the VM and observe resource states, use `make run-ossh` to have a QEMU shell.
