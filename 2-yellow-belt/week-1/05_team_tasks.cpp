#include <iostream>
#include <map>

using namespace std;

enum class TaskStatus {
	NEW,          // новая
	IN_PROGRESS,  // в разработке
	TESTING,      // на тестировании
	DONE          // завершена
};

// Объявляем тип-синоним для map<TaskStatus, int>,
// позволяющего хранить количество задач каждого статуса
using TasksInfo = map<TaskStatus, int>;

class TeamTasks {
public:

	// Получить статистику по статусам задач конкретного разработчика
	const TasksInfo& GetPersonTasksInfo(const string& person) const
	{ return m_tasks.at(person); }

	// Добавить новую задачу (в статусе NEW) для конкретного разработчитка
	void AddNewTask(const string& person)
	{ m_tasks[person][TaskStatus::NEW]++; }

	// Обновить статусы по данному количеству задач конкретного разработчика,
	// подробности см. ниже
	tuple<TasksInfo, TasksInfo>
	PerformPersonTasks(const string& person, int task_count) {
		TasksInfo  updated, untouched;

		if (m_tasks.count(person)) {
			TasksInfo& info = m_tasks.at(person);

			for (TaskStatus st = TaskStatus::NEW; st != TaskStatus::DONE && task_count; st = NextStatus(st)) {
				if (info.count(st)) {
					updated[NextStatus(st)] = min(info[st], task_count);
					task_count -= updated[NextStatus(st)];
				}
			}

			for (TaskStatus st = TaskStatus::NEW; st != NextStatus(TaskStatus::DONE); st = NextStatus(st)) {
				if (st != TaskStatus::DONE && info.count(st)) {
					if (updated.count(NextStatus(st))) {
						info[st] -= updated[NextStatus(st)];
						if (!info[st])
							info.erase(st);
					}
					if (info.count(st))
						untouched[st] = info[st];
				}

				if (updated.count(st))
					info[st] += updated[st];
			}
		}

		return make_tuple(updated, untouched);
	}

private:

	TaskStatus NextStatus(TaskStatus stat) const {
		return static_cast<TaskStatus>(static_cast<int>(stat) + 1);
	}

private:

	map<string, TasksInfo> m_tasks;
};

// Принимаем словарь по значению, чтобы иметь возможность
// обращаться к отсутствующим ключам с помощью [] и получать 0,
// не меняя при этом исходный словарь
void PrintTasksInfo(TasksInfo tasks_info) {
	cout << tasks_info[TaskStatus::NEW] << " new tasks" << ", "
		<< tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress" << ", "
		<< tasks_info[TaskStatus::TESTING] << " tasks are being tested" << ", "
		<< tasks_info[TaskStatus::DONE] << " tasks are done" << endl;
}

int main() {
	TeamTasks tasks;
	tasks.AddNewTask("Ilia");
	for (int i = 0; i < 3; ++i) {
		tasks.AddNewTask("Ivan");
	}
	cout << "Ilia's tasks: ";
	PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"));
	cout << "Ivan's tasks: ";
	PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"));

	TasksInfo updated_tasks, untouched_tasks;

	tie(updated_tasks, untouched_tasks) =
			tasks.PerformPersonTasks("Ivan", 2);
	cout << "Updated Ivan's tasks: ";
	PrintTasksInfo(updated_tasks);
	cout << "Untouched Ivan's tasks: ";
	PrintTasksInfo(untouched_tasks);

	tie(updated_tasks, untouched_tasks) =
			tasks.PerformPersonTasks("Ivan", 2);
	cout << "Updated Ivan's tasks: ";
	PrintTasksInfo(updated_tasks);
	cout << "Untouched Ivan's tasks: ";
	PrintTasksInfo(untouched_tasks);

	return 0;
}
