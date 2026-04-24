#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include "studentmanager.h"
#include "groupmanager.h"
#include "eventmanager.h"
#include "taskmanager.h"

class FileHandler {
    QString dataDir;
    QString studentsFile() const;
    QString groupsFile()   const;
    QString eventsFile()   const;
    QString tasksFile()    const;
    QString backupDir()    const;
public:
    explicit FileHandler(const QString& dir = "");
    void ensureDataDir();
    bool saveStudents(StudentManager& sm);
    bool saveGroups(GroupManager& gm);
    bool saveEvents(EventManager& em);
    bool saveTasks(TaskManager& tm);
    void saveAll(StudentManager& sm, GroupManager& gm, EventManager& em, TaskManager& tm);
    bool loadStudents(StudentManager& sm);
    bool loadGroups(GroupManager& gm);
    bool loadEvents(EventManager& em);
    bool loadTasks(TaskManager& tm);
    void loadAll(StudentManager& sm, GroupManager& gm, EventManager& em, TaskManager& tm);
    bool backupSystem();
    bool restoreSystem(StudentManager& sm, GroupManager& gm, EventManager& em, TaskManager& tm);
    bool exportReportTxt(StudentManager& sm, GroupManager& gm,
                         EventManager& em, TaskManager& tm, const QString& path);
};
#endif
