#ifndef TASKMANAGER_H
#define TASKMANAGER_H
#include <QString>
#include <QStringList>

struct TaskNode {
    int     id;
    QString groupId;      // SG001 format
    QString title;
    QString deadline;
    QString assignedMemberId;
    QString assignedBy;   // studentId or "ADMIN"
    int     progressPercent;
    QString notes;
    QString status;       // "Pending" | "InProgress" | "Completed"
    TaskNode* next;
    TaskNode(int i, QString gid, QString t, QString dl,
             QString mid, QString by, QString notes)
        : id(i), groupId(gid), title(t), deadline(dl),
          assignedMemberId(mid), assignedBy(by),
          progressPercent(0), notes(notes),
          status("Pending"), next(nullptr) {}
};

class TaskManager {
    TaskNode* head = nullptr;
    int nextId = 1;
public:
    TaskManager() = default;
    ~TaskManager();
    TaskManager(const TaskManager&) = delete;
    TaskManager& operator=(const TaskManager&) = delete;

    bool      createTask(QString groupId, QString title, QString deadline,
                         QString assignedMemberId, QString assignedBy, QString notes);
    TaskNode* findById(int id);
    bool      updateProgress(int id, int percent);
    bool      markCompleted(int id);
    bool      deleteTask(int id);

    QStringList getByGroup(const QString& groupId);
    QStringList getByGroupDetailed(const QString& groupId);
    QStringList getByMember(const QString& memberId);
    QStringList getPending();
    QStringList getAll();
    QStringList getAllDetailed();
    int         totalTasks();
    int         completedCount();
    double      completedPercent();
    void        sortByDeadline();

    TaskNode* getHead() const { return head; }
    void      appendRaw(TaskNode* node);
    void      setNextId(int id) { nextId = id; }
    int       getNextId() const { return nextId; }
};
#endif
