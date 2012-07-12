// Copyright 2011 The University of Michigan
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Authors - Jie Yu (jieyu@umich.edu)

// File: idiom/scheduler_main.cpp - The main entrance of the active scheduler.

#include "idiom/scheduler.hpp"

#include <csignal>

static idiom::Scheduler *sched_ctrl = new idiom::Scheduler;

static VOID I_InstrumentTrace(TRACE trace, VOID *v) {
  sched_ctrl->InstrumentTrace(trace, v);
}

static VOID I_ImageLoad(IMG img, VOID *v) {
  sched_ctrl->ImageLoad(img, v);
}

static VOID I_ImageUnload(IMG img, VOID *v) {
  sched_ctrl->ImageUnload(img, v);
}

static VOID I_SyscallEntry(THREADID tid, CONTEXT *ctxt, SYSCALL_STANDARD std,
                           VOID *v) {
  sched_ctrl->SyscallEntry(tid, ctxt, std, v);
}

static VOID I_SyscallExit(THREADID tid, CONTEXT *ctxt, SYSCALL_STANDARD std,
                          VOID *v) {
  sched_ctrl->SyscallExit(tid, ctxt, std, v);
}

static BOOL I_IntSignal(THREADID tid, INT32 sig, CONTEXT *ctxt, BOOL hasHandler,
                        const EXCEPTION_INFO *pExceptInfo, VOID *v) {
  sched_ctrl->IntSignal(tid, sig, ctxt, hasHandler, pExceptInfo, v);
  return FALSE;
}

static VOID I_ContextChange(THREADID tid, CONTEXT_CHANGE_REASON reason,
                            const CONTEXT *from, CONTEXT *to, INT32 info,
                            VOID *v) {
  sched_ctrl->ContextChange(tid, reason, from, to, info, v);
}

static VOID I_ProgramStart() {
  sched_ctrl->ProgramStart();
}

static VOID I_ProgramExit(INT32 code, VOID *v) {
  sched_ctrl->ProgramExit(code, v);
}

static VOID I_ThreadStart(THREADID tid, CONTEXT *ctxt, INT32 flags, VOID *v) {
  sched_ctrl->ThreadStart(tid, ctxt, flags, v);
}

static VOID I_ThreadExit(THREADID tid, const CONTEXT *ctxt, INT32 code,
                         VOID *v) {
  sched_ctrl->ThreadExit(tid, ctxt, code, v);
}

int main(int argc, char *argv[]) {
  sched_ctrl->Initialize();
  sched_ctrl->PreSetup();
  PIN_InitSymbols();
  PIN_Init(argc, argv);
  sched_ctrl->PostSetup();
  TRACE_AddInstrumentFunction(I_InstrumentTrace, NULL);
  IMG_AddInstrumentFunction(I_ImageLoad, NULL);
  IMG_AddUnloadFunction(I_ImageUnload, NULL);
  PIN_AddSyscallEntryFunction(I_SyscallEntry, NULL);
  PIN_AddSyscallExitFunction(I_SyscallExit, NULL);
  PIN_InterceptSignal(SIGUSR2, I_IntSignal, NULL);
  PIN_AddContextChangeFunction(I_ContextChange, NULL);
  PIN_AddFiniFunction(I_ProgramExit, NULL);
  PIN_AddThreadStartFunction(I_ThreadStart, NULL);
  PIN_AddThreadFiniFunction(I_ThreadExit, NULL);
  I_ProgramStart();
  // start the program
  PIN_StartProgram();
}
