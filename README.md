# todo
A command line program that allows you to manage and keep track of tasks in your daily life.
---
Commands

`todo ls (-i -com -c [category] -due [date] -cr [date created])`
- Lists your current tasks. Add `-i` to show extra info. Add `-com` to show completed tasks only.
- Filter tasks by add criteria such as category, due date, or creation date with `-c ... -due ... -cr ...`.

`todo com [index or name]`
- Complete a task! Provide either the task index (shown with `todo ls`) or the task name.
- Will also show you an encouraging message :)

`todo del [index or name]`
- Delete a task with the given index or name. Deleted tasks are removed forever, and will not be added to the completed task list.

`todo [task] (-c [category] -due [date] -n [note])`
- Simply type `todo` followed by the task message to create a new task!
- Add `-c` followed by a category name to add it to that category.
- Add `-due` followed by a date to set the due date of the task.
- Add `-n` followed by a note to add a note for the task.

All tasks are stored in a file called .todo and all completed tasks in .todo-com.
