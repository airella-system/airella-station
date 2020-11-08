#pragma once

#include <Arduino.h>

template <class Argument, class Progress, class Result>
class Task;

template <class Argument, class Progress, class Result>
class TaskRequestorHandle {
 public:
  TaskRequestorHandle(Task<Argument, Progress, Result>* task) { this->task = task; }
  void setProgress(Progress progress) { task->setProgress(progress); }

 private:
  Task<Argument, Progress, Result>* task;
};

class TaskState {
 public:
  enum State {
    PENDING = 0,
    IN_PROGRESS = 1,
    COMPLETED = 2,
  };
};

template <class Argument, class Progress, class Result>
class Task {
 public:
  Task() {
    stub = true;
  }

  Task(Result (*taskBody)(TaskRequestorHandle<Argument, Progress, Result>, Argument)) {
    notifySemaphore = xSemaphoreCreateMutex();
    dataSemaphore = xSemaphoreCreateMutex();
    this->taskBody = taskBody;

    state = new TaskState::State();
    *state = TaskState::PENDING;

    progress = new Progress();
    result = new Result();
    stub = false;
  }

  ~Task() {
    if (!stub) {
      delete state;
      delete progress;
      delete result;
    }
  }

  void lockData() { xSemaphoreTake(dataSemaphore, portMAX_DELAY); }

  void unlockData() { xSemaphoreGive(dataSemaphore); }

  void run(Argument arg) {
    lockData();
    *state = TaskState::IN_PROGRESS;
    unlockData();
    Result result = taskBody(TaskRequestorHandle<Argument, Progress, Result>(this), arg);
    lockData();
    std::memcpy(this->result, &result, sizeof(Result));
    *state = TaskState::COMPLETED;
    unlockData();
    notifyRequestor();
  }

  void setProgress(Progress progress) {
    lockData();
    std::memcpy(this->progress, &progress, sizeof(Progress));
    unlockData();
    notifyRequestor();
  }

  void notifyRequestor() { xSemaphoreGive(notifySemaphore); }

  void setStub(bool stub) { this->stub = stub; }

  Progress* getProgress() { return progress; }

  SemaphoreHandle_t getNotifySemaphore() { return notifySemaphore; }

  Result* getResult() { return result; }

  TaskState::State* getState() { return state; }

 private:
  TaskHandle_t requestor;
  SemaphoreHandle_t notifySemaphore;
  SemaphoreHandle_t dataSemaphore;
  Result (*taskBody)(TaskRequestorHandle<Argument, Progress, Result> task, Argument);
  Progress* progress;
  Result* result;
  TaskState::State* state;
  bool stub;
};
