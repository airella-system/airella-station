#pragma once

#include <Arduino.h>

class TaskHandler {};

template <class Argument, class Progress, class Result>
class Task {
  enum TaskState {
    PENDING = 0,
    IN_PROGRESS = 1,
    COMPLETED = 2,
  };

 public:
  Task() { stub = true; }

  Task(Result (*taskBody)(Task*, Argument)) {
    notifySemaphore = xSemaphoreCreateMutex();
    dataSemaphore = xSemaphoreCreateMutex();
    this->taskBody = taskBody;

    state = new TaskState();
    *state = PENDING;

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

  static int startBlocking(QueueHandle_t queue, Result (*taskBody)(Task*, Argument), void (*onProgress)(Progress),
                           void (*onComplete)(Result)) {
    Task<Argument, Progress, Result> task(taskBody);
    xSemaphoreTake(task.notifySemaphore, portMAX_DELAY);
    if (xQueueSend(queue, &task, 0) != pdTRUE) {
      return -1;
    }

    while (true) {
      xSemaphoreTake(task.notifySemaphore, portMAX_DELAY);
      task.lockData();
      TaskState taskState = *task.state;
      if (taskState == IN_PROGRESS) {
        Progress progress = *task.progress;
        task.unlockData();
        onProgress(progress);
      } else if (taskState == COMPLETED) {
        Result result = *task.result;
        task.unlockData();
        onComplete(result);
        break;
      } else {
        // this should never happen
        task.unlockData();
      }
    }
    return 0;
  }

  void lockData() { xSemaphoreTake(dataSemaphore, portMAX_DELAY); }

  void unlockData() { xSemaphoreGive(dataSemaphore); }

  void run(Argument arg) {
    lockData();
    *state = IN_PROGRESS;
    unlockData();
    Result result = taskBody(this, arg);
    lockData();
    std::memcpy(this->result, &result, sizeof(Result));
    *state = COMPLETED;
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

  void setStub(bool stub) {
    this->stub = stub;
  }

 private:
  TaskHandle_t requestor;
  SemaphoreHandle_t notifySemaphore;
  SemaphoreHandle_t dataSemaphore;
  Result (*taskBody)(Task* task, Argument);
  Progress* progress;
  Result* result;
  TaskState* state;
  bool stub;
};
