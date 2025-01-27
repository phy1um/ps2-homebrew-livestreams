#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <tamtypes.h>
#include <kernel.h>
#include <malloc.h>

#include <p2g/log.h>
#include <p2g/core.h>

#include "director.h"
#include "debug/debug.h"

#ifndef VBLANK_TIMEOUT_MAX
#define VBLANK_TIMEOUT_MAX 50
#endif

#define VU1DBG_STACK_SIZE 0x4000
#define PANIC_STACK_SIZE 0x1000

#define GS_REG_CSR ((volatile uint64_t *)0x12001000)

enum state {
  STATE_INVALID,
  STATE_RUN,
  STATE_RESUME,
  STATE_VU1DBG,
  STATE_PANIC,
};

static s32 panic_thread_id = 0;
static s32 main_thread_id = 0;
static s32 vu1dbg_thread_id = 0;

static int vblank_panic_counter = 0;
static enum state director_state = 0;

static int director_vblank_handler(int u) {
  switch (director_state) {
    // don't do other tests if we are debugging!
    case STATE_VU1DBG:
      iWakeupThread(vu1dbg_thread_id);
      ExitHandler();
      return 0;
    case STATE_RESUME:
      vblank_panic_counter = 0;
      director_state = STATE_RUN;
      iWakeupThread(main_thread_id);
      ExitHandler();
      return 0;
    default:
      break;
  }

  // test for VU1 T breakpoints
  if (vpu_stat() & 0x400) {
    director_state = STATE_VU1DBG; 
    iWakeupThread(vu1dbg_thread_id);
  }
  // if pending draw is not finished
  else if (!(*GS_REG_CSR & 2)) {
    if (vblank_panic_counter > VBLANK_TIMEOUT_MAX) {
      director_state = STATE_PANIC;
      iWakeupThread(panic_thread_id);
    } else {
      iRotateThreadReadyQueue(15);
    }
    vblank_panic_counter += 1;
  } else {
    vblank_panic_counter = 0;
    *GS_REG_CSR |= 2;
    iWakeupThread(main_thread_id);
  }
  ExitHandler();
  return 0;
}

static int on_panic(void *arg) {
  SleepThread();
  p2g_fatal("main loop panic");
  return 1;
}

static int vu1dbg_main(void *arg) {
  SleepThread();
  vpu_debugger();
  logerr("outside debugger");
  return 1;
}

int director_resume_run() {
  director_state = STATE_RESUME; 
  info("resume main: vblank panic counter = %d", vblank_panic_counter);
  return 0;
}

int director_run() {
  main_thread_id = GetThreadId();
  void *vu1dbg_stack = memalign(16, VU1DBG_STACK_SIZE);
  void *panic_stack = memalign(16, PANIC_STACK_SIZE);
  ee_thread_t vu1dbg_thread = {
    .func = vu1dbg_main,
    .stack = vu1dbg_stack,
    .stack_size = VU1DBG_STACK_SIZE,
    .gp_reg = &_gp,
    .initial_priority = 20,
  };
  vu1dbg_thread_id = CreateThread(&vu1dbg_thread);
  StartThread(vu1dbg_thread_id, 0);

  ee_thread_t panic_thread = {
    .func = on_panic,
    .stack = panic_stack,
    .stack_size = PANIC_STACK_SIZE,
    .gp_reg = &_gp,
    .initial_priority = 20,
  };
  panic_thread_id = CreateThread(&panic_thread);
  StartThread(panic_thread_id, 0);

  int rv = AddIntcHandler(2, director_vblank_handler, 0);
  if (rv == -1) {
    logerr("register INTC handler for VBLANK");
    return 1;
  }
  EnableIntc(2);
  vu1_enable_td_bits();
  return 0;
}

