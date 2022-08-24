#include "taskmanager.h"

#include "genesis.h"
#include "imgui/imgui_impl.h"
#include "memory.h"
#include "timer.h"

namespace Genesis
{

//-------------------------------------------------------------------------
// TaskManager

TaskManager::TaskManager()
    : mIsRunning(true)
{
}

TaskManager::~TaskManager()
{
    for (auto& pTask : mTasks)
    {
        delete pTask;
    }
}

void TaskManager::AddTask(const std::string& name, Task* task, TaskFunc func, TaskPriority priority)
{
    if (task == nullptr)
    {
        return;
    }

    TaskInfo* info = new TaskInfo();
    info->name = name;
    info->task = task;
    info->func = func;
    info->priority = priority;
    info->remove = false;

    // The task list is empty - just add it to the list, nothing else is
    // needed.
    if (mTasks.empty())
    {
        mTasks.push_back(info);
        return;
    }

    // Find where we should add our task. Note that the most prioritary
    // tasks have the lowest value (0 is updated before 20, for example)
    TaskInfoList::iterator it = mTasks.begin();
    TaskInfoList::iterator itEnd = mTasks.end();
    while (it != itEnd)
    {
        if ((*it)->priority > priority)
        {
            mTasks.insert(it, info);
            return;
        }
        it++;
    }

    // If every task has a lower priority value that our own, then we add
    // our task to the end of the list.
    mTasks.push_back(info);
}

void TaskManager::RemoveTask(Task* pTask) 
{
    for (auto& pTaskInfo : mTasks)
    {
        if (pTaskInfo->task == pTask)
        {
            pTaskInfo->remove = true;
            break;
        }
    }
}

void TaskManager::Update()
{
    m_Timer.Update();
    const float delta = m_Timer.GetDelta();

    bool tasksRemoved = false;
    for (auto& pTaskInfo : mTasks)
    {
        Task* task = pTaskInfo->task;
        TaskFunc func = pTaskInfo->func;
        if (pTaskInfo->remove || (*task.*func)(delta) == TaskStatus::Stop)
        {
            tasksRemoved = true;
            pTaskInfo->remove = true;
        }
    }

    // Only call RemoveMarkedTasks if any task has been stopped during
    // this update.
    if (tasksRemoved)
    {
        RemoveMarkedTasks();
    }
}

void TaskManager::RemoveMarkedTasks()
{
    TaskInfoList::iterator it = mTasks.begin();
    TaskInfoList::iterator itEnd = mTasks.end();
    while (it != itEnd)
    {
        if ((*it)->remove == false)
        {
            it++;
        }
        else
        {
            delete *it;
            it = mTasks.erase(it);
        }
    }
}
} // namespace Genesis