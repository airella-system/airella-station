#pragma once

#include <Arduino.h>
#include "Task.h"

template <class Argument, class Progress, class Result>
class TaskHandler {
 public:
  TaskHandler(int queueSize) { xTaskQueue = xQueueCreate(queueSize, sizeof(Task<Argument, Progress, Result>)); }

  void runPendingTasks(Argument argument) {
    Task<Argument, Progress, Result> task;
    while (xQueueReceive(xTaskQueue, &task, 0) == pdTRUE) {
      task.setStub(true);
      task.run(argument);
    }
  }

  int startBlockingTask(Result (*taskBody)(TaskRequestorHandle<Argument, Progress, Result>, Argument), void (*onProgress)(Progress), void (*onComplete)(Result)) {
    Task<Argument, Progress, Result> task(taskBody);
    xSemaphoreTake(task.getNotifySemaphore(), portMAX_DELAY);
    if (xQueueSend(xTaskQueue, &task, 0) != pdTRUE) {
      return -1;
    }

    while (true) {
      xSemaphoreTake(task.getNotifySemaphore(), portMAX_DELAY);
      task.lockData();
      TaskState::State taskState = *task.getState();
      if (taskState == TaskState::IN_PROGRESS) {
        Progress progress = *task.getProgress();
        task.unlockData();
        onProgress(progress);
      } else if (taskState == TaskState::COMPLETED) {
        Result result = *task.getResult();
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

 private:
  QueueHandle_t xTaskQueue = NULL;
};
